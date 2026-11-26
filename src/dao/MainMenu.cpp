// The Escape menu's buttons (dao/MainMenu.h).
//
// Notes on the menu: read 2026-11-26 in mainmenu.gfx as the game takes it
// (packages/core/patch/patch003.erf, decompiled with FFDec 26.3.0), against dev/GUI.md's scenes
// (MainMenu 0x14, opened by "ToggleFullScreenGUIMessage MainMenu") and dev/message_handlers.csv.
// The class MainMenu gives each button its text, a string id of the movie's Localization, and its
// action:
// - ResumeGame_mc (336209): "GameModeController.HideGUIScreen MainMenu".
// - SaveGame_mc (336194): MainMenu.HideContents, "SaveLoad.SetSaveLoadMode 1",
//   "GameModeController.PushGUIScreen SaveLoad". It is enabled only while SaveLoad.SavingEnabled
//   reads 1.
// - LoadGame_mc (169422): the same with mode 2.
// - Options_mc (169423) and Screenshots_mc (378170, the online features): HideContents and
//   PushGUIScreen with Options and GUICommunityFeatures.
// - Exit_mc (336210): a box of the movie's own, Popups.ShowDialogBox with the question 325620 and
//   the buttons 363223 and 325484, whose first sends "GameModeController.HideGUIScreen MainMenu"
//   and "GameModeController.ExitToStartMenuMessage". The box's callbacks are the movie's
//   closures, which nothing outside it can call, so Exit here sends the two messages itself and
//   ExitQuestion gives the question's text for a caller to ask.
// The buttons sit under clips the movie does not name (Menu_mc.instance8 and on, seen in play),
// so they are not read or pressed by a path as the start menu's are: the texts come from the
// talk table and the actions are sent as the buttons send them. ExitToStartMenuMessage has its
// handlers in GameModeGUI and GameModeCharGen alone (0x006400C0), so it is sent while the menu is
// still up: sent a second and a half after HideGUIScreen, in explore mode, it did nothing (seen).
//
// Seen in play 2026-11-26 through the Eluvian server, Steam's game on Windows 11, a new game in
// Castle Cousland: "GUI.ToggleFullScreenGUI MainMenu" in the GUI mode with MainMenu the
// full-screen interface, the game paused and GUI.SelectedBookPage 20; HideGUIScreen MainMenu
// back in explore mode and running; HideContents, SetSaveLoadMode 1 and PushGUIScreen SaveLoad
// with SaveLoad the full-screen interface and SaveLoad.SaveFileList led by its "New Save" row,
// and HideGUIScreen SaveLoad back at the menu with its contents shown; PushGUIScreen Options and
// HideGUIScreen Options the same. SaveLoad.SavingEnabled read 0 while a training step held the
// game and 1 after. Then with examples/mainmenu, a save loaded for each start: the six buttons
// logged with their texts ("Resume Game" to "Exit Game") and the question as the menu opened;
// "savegame" chosen from Open's menu with SaveLoad the full-screen interface after; and "exit"
// chosen, its two messages sent in the one frame, with the game at the start menu.
#include "dao/MainMenu.h"

#include "dao/Data.h"
#include "dao/Gui.h"
#include "dao/Messages.h"
#include "dao/Tlk.h"

#include <array>

namespace dao::mainmenu
{

namespace
{

struct Button
{
    Entry entry;
    std::uint32_t label; // the string id the movie's Localization gives the button
};

constexpr std::array<Button, 6> kButtons{{
    {Entry::Resume, 336209},
    {Entry::SaveGame, 336194},
    {Entry::LoadGame, 169422},
    {Entry::Options, 169423},
    {Entry::PlayerProfile, 378170},
    {Entry::Exit, 336210},
}};

constexpr std::uint32_t kExitQuestion = 325620;

bool Enabled(Entry entry)
{
    return entry != Entry::SaveGame || data::Number(L"SaveLoad.SavingEnabled").value_or(0) == 1;
}

// Hides the menu's buttons as the movie does before it opens another screen over them.
void HideContents()
{
    if (auto menu = gui::ScreenMovie(L"MainMenu"))
    {
        menu->Invoke("MainMenu.HideContents");
    }
}

} // namespace

bool Shown()
{
    const auto full = gui::CurrentFullScreen();
    return full && full->name == L"MainMenu";
}

bool Open()
{
    return !Shown() && messages::SendText(L"GUI.ToggleFullScreenGUI MainMenu");
}

std::vector<Item> Items()
{
    std::vector<Item> items;
    if (!Shown())
    {
        return items;
    }
    for (const Button &button : kButtons)
    {
        items.push_back({button.entry, tlk::String(button.label), Enabled(button.entry)});
    }
    return items;
}

std::wstring ExitQuestion()
{
    return tlk::String(kExitQuestion);
}

Result Choose(Entry entry)
{
    if (!Shown())
    {
        return Result::NotShown;
    }
    if (!Enabled(entry))
    {
        return Result::Disabled;
    }
    switch (entry)
    {
    case Entry::Resume:
        messages::SendText(L"GameModeController.HideGUIScreen MainMenu");
        break;
    case Entry::SaveGame:
        HideContents();
        messages::SendText(L"SaveLoad.SetSaveLoadMode 1");
        messages::SendText(L"GameModeController.PushGUIScreen SaveLoad");
        break;
    case Entry::LoadGame:
        HideContents();
        messages::SendText(L"SaveLoad.SetSaveLoadMode 2");
        messages::SendText(L"GameModeController.PushGUIScreen SaveLoad");
        break;
    case Entry::Options:
        HideContents();
        messages::SendText(L"GameModeController.PushGUIScreen Options");
        break;
    case Entry::PlayerProfile:
        HideContents();
        messages::SendText(L"GameModeController.PushGUIScreen GUICommunityFeatures");
        break;
    case Entry::Exit:
        messages::SendText(L"GameModeController.HideGUIScreen MainMenu");
        messages::SendText(L"GameModeController.ExitToStartMenuMessage");
        break;
    }
    return Result::Chosen;
}

} // namespace dao::mainmenu
