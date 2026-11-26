// The inventory as its screen lists it: the party's pack and its plot items, each row an item
// with the name and text the screen shows, what the screen's character wears, the pack's room
// and the party's money.
//
//   for (const dao::inventory::Row &row : dao::inventory::Backpack())
//       log(row.item.name, row.item.stack);
//   if (const auto sword = dao::inventory::Worn(0)) // the main hand
//       log(sword->name);
#pragma once

#include "dao/ObjectId.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace dao::inventory
{

// An item as the screen shows it.
struct Item
{
    ObjectId id = kInvalidObject;
    std::wstring name;        // L"Lesser Health Poultice"
    std::wstring description; // its text in the screen's examine box
    std::wstring tag;
    std::wstring icon;
    std::int32_t stack = 0;            // how many the item stands for
    std::int32_t cost = 0;             // its Cost, as the screen's tooltip counts it
    bool isNew = false;                // the screen still marks it as newly got
    std::uint32_t equippableSlots = 0; // a bit for each INVENTORY_SLOT_* it fits; 0 for none
};

// A row of one of the screen's lists.
struct Row
{
    Item item;
    std::int32_t groupSize = 0; // how many items the screen folds into this row
};

// Returns every item of the party's pack, plot items left out, whether or not the screen is up.
//
// Call it, and the rest, on the game thread, with a game in play.
std::vector<Item> Pack();

// Returns the party's pack as the screen lists it, in the screen's order and with its filter.
// @return the rows; empty while the inventory screen is not up, which is when the game builds them
std::vector<Row> Backpack();

// Returns the party's plot items as the screen lists them, empty while the screen is not up.
std::vector<Row> PlotItems();

// Returns what the screen's character (dao/Party.h) wears in a slot.
// @param slot an INVENTORY_SLOT_*: 0 the main hand, 1 the off hand, 4 the chest
// @return nullopt for an empty slot
std::optional<Item> Worn(std::int32_t slot);

// How full the party's pack is.
struct Room
{
    std::int32_t used = 0;
    std::int32_t most = 0;
};

// Returns the pack's room, as the screen's counter shows it.
Room PackRoom();

// Returns the party's money in copper: a hundred to a silver, a hundred silver to a sovereign.
std::int32_t Money();

} // namespace dao::inventory
