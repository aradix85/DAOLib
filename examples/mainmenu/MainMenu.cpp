// Example: the Escape menu's buttons (dao/MainMenu.h).
//
// Each time the menu opens, its buttons are logged from the top with their texts and whether the
// menu lets each be pressed, with the question it asks before Exit. What it logs goes to
// example_mainmenu.log.
//
// To have it press something, with nobody at the keyboard, put a button's name as
// dao::mainmenu::Name gives it ("resume", "savegame", "options", "exit" ...) in a file named
// example_mainmenu.choose beside the logs before the game starts: the first time the game is in
// explore mode the menu is opened, and the button pressed once it is up.
#include "dao/MainMenu.h"
#include "dao/ClientExoApp.h"
#include "dao/Frame.h"
#include "dao/Gui.h"
#include "dao/Log.h"
#include "dao/Plugin.h"
#include "dao/String.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <string>
#include <utility>

namespace
{

namespace menu = dao::mainmenu;

std::string g_choice; // the file's line, emptied once acted on
bool g_opened = false;

std::string_view Said(menu::Result result)
{
    switch (result)
    {
    case menu::Result::Chosen:
        return "chosen";
    case menu::Result::Disabled:
        return "refused: the menu shows it disabled";
    default:
        return "refused: the menu is not up";
    }
}

void Shown()
{
    spdlog::info("the menu is up");
    for (const menu::Item &item : menu::Items())
    {
        spdlog::info("{}: {}{}", menu::Name(item.entry), dao::Utf8(item.label), item.enabled ? "" : ", disabled");
    }
    spdlog::info("before exit it asks: {}", dao::Utf8(menu::ExitQuestion()));
    const std::string choice = std::exchange(g_choice, {});
    for (const menu::Item &item : menu::Items())
    {
        if (menu::Name(item.entry) == choice)
        {
            spdlog::info("{}: {}", choice, Said(menu::Choose(item.entry)));
        }
    }
}

void Frame()
{
    if (!g_choice.empty() && !g_opened && dao::CClientExoApp::CurrentMode() == dao::GameMode::Explore)
    {
        g_opened = true;
        spdlog::info("in explore mode; the menu opened: {}", menu::Open());
    }
}

} // namespace

extern "C" __declspec(dllexport) bool __cdecl DAOLib_Load(const dao::LoaderInfo *info)
{
    dao::OpenLog("example_mainmenu");
    if (info == nullptr || info->apiVersion != dao::kPluginApiVersion || !info->knownExecutable)
    {
        spdlog::error("not the loader or the game this was built for");
        return false;
    }
    std::ifstream file(dao::LogDirectory() / L"example_mainmenu.choose");
    std::getline(file, g_choice);
    dao::OnFrame(&Frame);
    dao::gui::OnFullScreen([](std::wstring_view name, bool shown) {
        if (shown && name == L"MainMenu")
        {
            Shown();
        }
        else
        {
            spdlog::info("{} {}", dao::Utf8(name), shown ? "opened" : "closed");
        }
    });
    return true;
}
