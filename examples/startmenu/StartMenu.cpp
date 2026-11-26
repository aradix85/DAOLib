// Example: the start menu's buttons, its other campaigns and the campaign chooser
// (dao/StartMenu.h).
//
// Each time the start menu comes up with its buttons shown, the buttons are logged from the top,
// each with its text and whether the menu shows it and lets it be pressed, then the campaigns
// Other Campaigns lists and the two the chooser offers. What it logs goes to
// example_startmenu.log.
//
// To have it press something, with nobody at the mouse, put one line in a file named
// example_startmenu.choose beside the logs before the game starts:
// - a button's name as dao::startmenu::Name gives it ("options", "quit", "loadgame" ...): the
//   button is pressed once, and the log says what the menu made of it;
// - "origins" or "awakening": New Game is pressed, and when the chooser comes up that campaign is
//   started from it;
// - "close": New Game is pressed, and the chooser is closed again;
// - "play <name>": the other campaign of that name is started, as its row and Play do.
#include "dao/StartMenu.h"
#include "dao/Frame.h"
#include "dao/Log.h"
#include "dao/Plugin.h"
#include "dao/String.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace
{

namespace menu = dao::startmenu;

bool g_wasReady = false;
std::string g_choice;                      // the file's line, emptied once acted on
std::optional<menu::MainCampaign> g_start; // the campaign to start when the chooser is up
bool g_close = false;                      // close the chooser when it is up

std::string_view Said(menu::Result result)
{
    switch (result)
    {
    case menu::Result::Chosen:
        return "chosen";
    case menu::Result::Hidden:
        return "refused: the menu does not show it";
    case menu::Result::Disabled:
        return "refused: the menu shows it disabled";
    case menu::Result::Unknown:
        return "refused: no campaign of that name";
    default:
        return "refused: not up";
    }
}

void LogMenu()
{
    for (const menu::Item &item : menu::Items())
    {
        spdlog::info("{}: \"{}\"{}{}", menu::Name(item.entry), dao::Utf8(item.label), item.shown ? "" : ", hidden",
                     item.enabled ? "" : ", disabled");
    }
    for (const menu::Campaign &campaign : menu::Campaigns())
    {
        spdlog::info("other campaign {}: \"{}\"", dao::Utf8(campaign.name), dao::Utf8(campaign.displayName));
    }
    spdlog::info(R"(the chooser's campaigns: "{}" and "{}")", dao::Utf8(menu::Name(menu::MainCampaign::Origins)),
                 dao::Utf8(menu::Name(menu::MainCampaign::Awakening)));
}

void Act()
{
    const std::string choice = std::exchange(g_choice, {});
    if (choice.empty())
    {
        return;
    }
    if (choice == "origins" || choice == "awakening" || choice == "close")
    {
        g_close = choice == "close";
        if (!g_close)
        {
            g_start = choice == "origins" ? menu::MainCampaign::Origins : menu::MainCampaign::Awakening;
        }
        spdlog::info("newgame, for \"{}\": {}", choice, Said(menu::Choose(menu::Entry::NewGame)));
        return;
    }
    if (choice.starts_with("play "))
    {
        spdlog::info("{}: {}", choice, Said(menu::Play(dao::Wide(choice.substr(5)))));
        return;
    }
    for (const menu::Item &item : menu::Items())
    {
        if (menu::Name(item.entry) == choice)
        {
            spdlog::info("{}: {}", choice, Said(menu::Choose(item.entry)));
            return;
        }
    }
    spdlog::warn("\"{}\" names no button", choice);
}

void Frame()
{
    if (menu::ChooserShown() && (g_start || g_close))
    {
        if (g_close)
        {
            spdlog::info("the chooser is up; closed: {}", Said(menu::CloseChooser()));
        }
        else
        {
            spdlog::info("the chooser is up; \"{}\": {}", dao::Utf8(menu::Name(*g_start)), Said(menu::Start(*g_start)));
        }
        g_start.reset();
        g_close = false;
        return;
    }
    const bool ready = menu::Ready();
    if (ready && !g_wasReady)
    {
        spdlog::info("the start menu is up");
        LogMenu();
        Act();
    }
    g_wasReady = ready;
}

} // namespace

extern "C" __declspec(dllexport) bool __cdecl DAOLib_Load(const dao::LoaderInfo *info)
{
    dao::OpenLog("example_startmenu");
    if (info == nullptr || info->apiVersion != dao::kPluginApiVersion || !info->knownExecutable)
    {
        spdlog::error("not the loader or the game this was built for");
        return false;
    }
    std::ifstream file(dao::LogDirectory() / L"example_startmenu.choose");
    std::getline(file, g_choice);
    dao::OnFrame(&Frame);
    return true;
}
