// The start menu's buttons, its other campaigns and the campaign chooser (dao/StartMenu.h).
//
// Notes on the start menu: read 2026-11-26 (DAOLib #344) in startmenu2.gfx and newgame.gfx as
// the game takes them (packages/core/patch/patch003.erf, decompiled with FFDec 26.3.0), against
// dev/GUI.md's scenes and fscommands and dev/message_handlers.csv. The scene StartMenu (0x12,
// startmenu2, layer 3) is shown by GameModePregame. Its class StartMenu hides the buttons'
// clip until the token GameMode.ShowStartMenuContents is not 0, and gives each button its text
// and its action:
// - ResumeButton: "SaveLoad.ResumeGame" (SaveLoadProxy, 0x006D15E0). UpdateLoadButtons hides it
//   and disables LoadGameButton while SaveLoad.HeroList has no member.
// - NewGameButton: fscommand LoadModule "single player" (0x0051B8D0), which asks for the scene
//   NewGame when DAO_PRC_EP_1 is enabled and loads the campaign otherwise (dev/GUI.md).
// - LoadGameButton: HideContents, "SaveLoad.SetSaveLoadMode 2", "GameModeController.PushGUIScreen
//   SaveLoad".
// - ModuleButton (Other Campaigns): the scene ModuleList, disabled while Client.ModuleInfoList
//   has no member. Its list is that attribute's members, each a Name, a DisplayName and a
//   Description; a row's click sets StartMenu.SelectedModule to the Name, and Play is
//   LoadSelectedModule, fscommand LoadModule with it. The list's rows and its Play and Back
//   buttons sit under clips the movie does not name (instance51, instance58), so Play here sets
//   the variable and calls the function, which needs no page shown.
// - DownloadableContentButton, ScreenshotsButton, OptionsButton: PushGUIScreen with
//   PRCContentManager, GUICommunityFeatures and Options, the last two after HideContents.
// - CreditsButton: "GameModePregame.RefreshCreditEntries", then HideContents and
//   "GameModePregame.ShowCredits 0" with at most one member in GameModePregame.CreditEntryNames,
//   the ModuleList scene over that list otherwise.
// - QuitButton: fscommand QuitGame (0x0051BB50).
// HideContents hides _root.StartButtons_mc, the buttons' parent. StartMenu.ShowContents shows it
// again and has no caller in the movie; who calls it as the opened screen closes was not read.
// A button is a SharedLibrary.PushButton: Disable clears its `enabled` (GUI.Element's
// DisableEvents), MovieUtils.HideMovie its `_visible`, and SimulateButtonPush
// (Behaviour.PushButton) is the press and the release a click gives, with the click's sound,
// taken only while `enabled`. It does not look at `_visible`: the hidden
// Resume button reads enabled, so Choose refuses a hidden button itself. A button's text is at
// RollOver_mc.ButtonText_MC.TextField.text, and stays there while the button is disabled.
//
// The chooser (scene NewGame 0x3F, newgame.gfx): its two buttons set NewGame.ms_nGameSelection
// to 0 or 1 and its Close to -1, and each plays the timeline's Outro, whose end calls
// NewGame.TL_OnOutroComplete: "GUI.HideGUIScreen NewGame", then fscommand LoadModule with
// DAConstants.DAO_MODULE_NAME or EP1_MODULE_NAME for 0 or 1. Its labels are
// ExternalInterface.call("GetMainModuleName", 0 | 1). The buttons sit under unnamed clips, and
// PlayOutro called while the chooser's intro still plays does nothing (the Eluvian bridge's
// game_new calls it again after four seconds), so Start and CloseChooser set the selection and
// call TL_OnOutroComplete themselves.
//
// Seen in play 2026-11-26 through the Eluvian server, Steam's game (the Steamless exe with the
// 4 GB flag) on Windows 11, a few dozen mods installed and no saved game: the menu in pregame
// with ShowStartMenuContents 1 at the first read, 16 to 24 s after the launch; HeroList empty,
// ResumeButton hidden and LoadGameButton's enabled false, and a push of it changing nothing;
// ModuleInfoList's five campaigns and ModuleButton's push playing the list's page (the root's
// frame 274), StartMenu.Back the menu's again (241); NewGameButton's push showing NewGame, its
// Close button's push closing it in 0.4 s with the menu's buttons shown; the selection 0 and
// TL_OnOutroComplete, called as the chooser appeared (its frame 3), in loading at 0.4 s and
// character creation at 5.1 s; StartMenu.SelectedModule "DAO_PRC_LEL" and LoadSelectedModule in
// loading at 0.5 s and Leliana's Song's first conversation at 4.9 s; GetMainModuleName called
// from the start menu's movie answering "Dragon Age: Origins" and "Dragon Age: Awakening";
// QuitButton's push ending the process in 2.8 s. The buttons took a push from the root's frame
// 5 on, while the menu's intro still played. Then with examples/startmenu beside the Eluvian
// server and the turn-based mod, a start of the game for each line of its file: the nine
// buttons logged with their texts, Resume hidden and Load Game disabled, the five campaigns and
// the chooser's two names; "loadgame" refused as disabled and "resume" as hidden, the menu
// left as it was; "close" with the chooser closed and the menu up; "origins" in character
// creation 7 s after the menu came up; "play DAO_PRC_LEL" in that campaign's first conversation
// after 3 s; "quit" with the process gone. Not pressed: Resume and Load Game with a save,
// Downloadable Content, Player Profile, Options and Credits.
#include "dao/StartMenu.h"

#include "dao/Data.h"
#include "dao/Gui.h"
#include "dao/String.h"

#include <array>
#include <format>
#include <optional>

namespace dao::startmenu
{

namespace
{

struct Button
{
    Entry entry;
    std::string_view clip; // the button's name under kButtons
};

// From the top, as the menu lays them out.
constexpr std::array<Button, 9> kButtonOf{{
    {Entry::Resume, "ResumeButton"},
    {Entry::NewGame, "NewGameButton"},
    {Entry::LoadGame, "LoadGameButton"},
    {Entry::OtherCampaigns, "ModuleButton"},
    {Entry::DownloadableContent, "DownloadableContentButton"},
    {Entry::PlayerProfile, "ScreenshotsButton"},
    {Entry::Options, "OptionsButton"},
    {Entry::Credits, "CreditsButton"},
    {Entry::Quit, "QuitButton"},
}};

constexpr std::string_view kContents = "_root.StartButtons_mc";
constexpr std::string_view kButtons = "_root.StartButtons_mc.StartMenuButtons_mc";

std::optional<gui::Movie> Menu()
{
    return gui::ScreenMovie(L"StartMenu");
}

std::optional<gui::Movie> Chooser()
{
    return gui::ScreenMovie(L"NewGame");
}

bool ContentsShown(const gui::Movie &menu)
{
    return data::Number(L"GameMode.ShowStartMenuContents").value_or(0) != 0 &&
           menu.GetBool(std::format("{}._visible", kContents).c_str()).value_or(false);
}

Item Read(const gui::Movie &menu, const Button &button)
{
    Item item;
    item.entry = button.entry;
    const std::string clip = std::format("{}.{}", kButtons, button.clip);
    if (const auto label = menu.Get((clip + ".RollOver_mc.ButtonText_MC.TextField.text").c_str()))
    {
        item.label = Wide(label->text);
    }
    item.shown = menu.GetBool((clip + "._visible").c_str()).value_or(false);
    item.enabled = menu.GetBool((clip + ".enabled").c_str()).value_or(false);
    return item;
}

// Ends the chooser as its outro's end does, with a button's selection.
Result EndChooser(int selection)
{
    auto chooser = Chooser();
    if (!chooser)
    {
        return Result::NotShown;
    }
    chooser->SetVariable("NewGame.ms_nGameSelection", gui::Value::Number(selection));
    return chooser->Invoke("NewGame.TL_OnOutroComplete") ? Result::Chosen : Result::NotShown;
}

} // namespace

bool Ready()
{
    const auto menu = Menu();
    return menu && ContentsShown(*menu);
}

std::vector<Item> Items()
{
    std::vector<Item> items;
    const auto menu = Menu();
    if (!menu || !ContentsShown(*menu))
    {
        return items;
    }
    for (const Button &button : kButtonOf)
    {
        items.push_back(Read(*menu, button));
    }
    return items;
}

Result Choose(Entry entry)
{
    auto menu = Menu();
    if (!menu || !ContentsShown(*menu))
    {
        return Result::NotShown;
    }
    for (const Button &button : kButtonOf)
    {
        if (button.entry != entry)
        {
            continue;
        }
        const Item item = Read(*menu, button);
        if (!item.shown)
        {
            return Result::Hidden;
        }
        if (!item.enabled)
        {
            return Result::Disabled;
        }
        const std::string push = std::format("{}.{}.SimulateButtonPush", kButtons, button.clip);
        return menu->Invoke(push.c_str()) ? Result::Chosen : Result::NotShown;
    }
    return Result::NotShown;
}

std::vector<Campaign> Campaigns()
{
    std::vector<Campaign> campaigns;
    const std::size_t count = data::MemberCount(L"Client.ModuleInfoList").value_or(0);
    for (std::size_t i = 0; i < count; ++i)
    {
        const std::wstring member = std::format(L"Client.ModuleInfoList.{}.", i);
        campaigns.push_back({data::Text(member + L"Name").value_or(L""),
                             data::Text(member + L"DisplayName").value_or(L""),
                             data::Text(member + L"Description").value_or(L"")});
    }
    return campaigns;
}

Result Play(std::wstring_view name)
{
    auto menu = Menu();
    if (!menu)
    {
        return Result::NotShown;
    }
    bool listed = false;
    for (const Campaign &campaign : Campaigns())
    {
        listed = listed || campaign.name == name;
    }
    if (!listed)
    {
        return Result::Unknown;
    }
    const std::string selected = Utf8(name);
    menu->SetVariable("StartMenu.SelectedModule", gui::Value::String(selected.c_str()));
    return menu->Invoke("StartMenu.LoadSelectedModule") ? Result::Chosen : Result::NotShown;
}

bool ChooserShown()
{
    return Chooser().has_value();
}

std::wstring Name(MainCampaign campaign)
{
    auto movie = Chooser();
    if (!movie)
    {
        movie = Menu();
    }
    if (!movie)
    {
        return {};
    }
    const std::array<gui::Value, 2> args{gui::Value::String("GetMainModuleName"),
                                         gui::Value::Number(static_cast<double>(campaign))};
    const auto name = movie->Call("flash.external.ExternalInterface.call", args);
    return name ? Wide(name->text) : std::wstring{};
}

Result Start(MainCampaign campaign)
{
    return EndChooser(static_cast<int>(campaign));
}

Result CloseChooser()
{
    return EndChooser(-1);
}

} // namespace dao::startmenu
