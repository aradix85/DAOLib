// The menu Escape opens in a game (the game calls it MainMenu): its entries as it shows them, and
// each done as its button does it.
//
//   dao::mainmenu::Open();
//   for (const dao::mainmenu::Item &item : dao::mainmenu::Items())
//       log(item.label, item.enabled);
//   dao::mainmenu::Choose(dao::mainmenu::Entry::SaveGame);
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace dao::mainmenu
{

// The menu's buttons, from the top.
enum class Entry : std::uint8_t
{
    Resume,        // closes the menu
    SaveGame,      // opens the save screen; disabled where the game allows no save
    LoadGame,      // opens the load screen
    Options,       // opens the options screen
    PlayerProfile, // opens the online features' screen
    Exit,          // leaves the game in play for the start menu
};

// Returns "resume", "savegame", "loadgame", "options", "playerprofile" or "exit", for logging.
constexpr std::string_view Name(Entry entry)
{
    switch (entry)
    {
    case Entry::Resume:
        return "resume";
    case Entry::SaveGame:
        return "savegame";
    case Entry::LoadGame:
        return "loadgame";
    case Entry::Options:
        return "options";
    case Entry::PlayerProfile:
        return "playerprofile";
    default:
        return "exit";
    }
}

// One of the menu's buttons.
struct Item
{
    Entry entry = Entry::Exit;
    std::wstring label;   // the button's text, in the player's language
    bool enabled = false; // a click presses it; a disabled one is drawn greyed
};

// Returns whether the menu is the full-screen interface that is up.
//
// Call it, and the rest, on the game thread.
bool Shown();

// Opens the menu, as Escape does with nothing else open. The game pauses and enters its GUI mode.
// @return false, with nothing done, when the menu is up already or no game is in play
bool Open();

// Returns the menu's six buttons from the top, as it shows them.
// @return the buttons; empty when the menu is not Shown
std::vector<Item> Items();

// Returns the question the menu asks before Exit, in the player's language. Choose asks nothing:
// a plugin that leaves on the player's word asks this first.
std::wstring ExitQuestion();

// What a choice did.
enum class Result : std::uint8_t
{
    Chosen,   // done, as the button does it
    Disabled, // the menu shows that button greyed, so nothing was done
    NotShown, // the menu is not up, so nothing was done
};

// Does what one of the menu's buttons does. Exit leaves for the start menu at once, without the
// menu's question (ExitQuestion).
Result Choose(Entry entry);

} // namespace dao::mainmenu
