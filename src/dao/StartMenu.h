// The start menu: its entries as it shows them, a choice made as its button makes it, the other
// campaigns it lists, and the campaign chooser New Game opens.
//
//   if (dao::startmenu::Ready())
//       for (const dao::startmenu::Item &item : dao::startmenu::Items())
//           log(item.label, item.shown, item.enabled);
//   dao::startmenu::Choose(dao::startmenu::Entry::LoadGame);
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace dao::startmenu
{

// The menu's buttons, from the top.
enum class Entry : std::uint8_t
{
    Resume,              // loads the save written last; hidden while no character has a save
    NewGame,             // starts the campaign, or opens the campaign chooser when Awakening is enabled
    LoadGame,            // opens the load screen; disabled while no character has a save
    OtherCampaigns,      // lists the other campaigns (Campaigns); disabled while there are none
    DownloadableContent, // opens the downloadable content screen
    PlayerProfile,       // opens the online features' screen
    Options,             // opens the options screen
    Credits,             // plays the credits, or lists them when add-ins bring their own
    Quit,                // leaves the game, with no question asked
};

// Returns "resume", "newgame", "loadgame", "othercampaigns", "downloadablecontent",
// "playerprofile", "options", "credits" or "quit", for logging.
constexpr std::string_view Name(Entry entry)
{
    switch (entry)
    {
    case Entry::Resume:
        return "resume";
    case Entry::NewGame:
        return "newgame";
    case Entry::LoadGame:
        return "loadgame";
    case Entry::OtherCampaigns:
        return "othercampaigns";
    case Entry::DownloadableContent:
        return "downloadablecontent";
    case Entry::PlayerProfile:
        return "playerprofile";
    case Entry::Options:
        return "options";
    case Entry::Credits:
        return "credits";
    default:
        return "quit";
    }
}

// One of the menu's buttons, as the menu shows it now.
struct Item
{
    Entry entry = Entry::Quit;
    std::wstring label;   // the button's text, in the player's language
    bool shown = false;   // the menu draws it
    bool enabled = false; // a click presses it; a disabled one is drawn greyed
};

// Returns whether the start menu is up with its buttons shown. It is not while the intro movies
// play, nor while a screen the menu opened (the options, the load screen) covers it.
//
// Call it, and the rest, on the game thread.
bool Ready();

// Returns the menu's nine buttons from the top, each as the menu shows it now.
// @return the buttons; empty when the menu is not Ready
std::vector<Item> Items();

// What a choice did.
enum class Result : std::uint8_t
{
    Chosen,   // done, as the click does it
    Hidden,   // the menu does not show that button, so nothing was done
    Disabled, // the menu shows that button greyed, so nothing was done
    Unknown,  // the menu lists no campaign of that name, so nothing was done
    NotShown, // the menu, or the chooser, is not up, so nothing was done
};

// Presses one of the menu's buttons, as a click does, with the button's sound.
Result Choose(Entry entry);

// A campaign the menu's Other Campaigns lists.
struct Campaign
{
    std::wstring name;        // the campaign's own name, for Play: L"DAO_PRC_LEL"
    std::wstring displayName; // as the list shows it: L"Leliana's Song"
    std::wstring description; // as the list shows it beside a chosen campaign; often empty
};

// Returns the campaigns Other Campaigns lists, in its order.
// @return the campaigns; empty before the game has listed them, or when there are none
std::vector<Campaign> Campaigns();

// Starts one of the other campaigns, as choosing it in the list and pressing Play does.
// @param name the campaign's Campaign::name
//
// The start menu must be up, on either of its pages.
Result Play(std::wstring_view name);

// The two campaigns the campaign chooser offers, by its buttons' numbers.
enum class MainCampaign : std::uint8_t
{
    Origins = 0,
    Awakening = 1,
};

// Returns whether the campaign chooser is up: the box New Game opens when Awakening is enabled.
bool ChooserShown();

// Returns a main campaign's name as the chooser labels it, in the player's language.
// @return the name; empty when neither the start menu nor the chooser is up
std::wstring Name(MainCampaign campaign);

// Starts a main campaign from the chooser, as its button does, without the chooser's outro.
Result Start(MainCampaign campaign);

// Closes the chooser as its Close button does, without its outro. The start menu stays up.
Result CloseChooser();

} // namespace dao::startmenu
