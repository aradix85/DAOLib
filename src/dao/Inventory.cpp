// The inventory's lists (dao/Inventory.h).
//
// Notes on the inventory: read 2026-11-26 in inventory.gfx as the game takes it
// (packages/core/patch/patch003.erf, decompiled with FFDec 26.3.0), with dev/GUI.md's and
// dev/ITEMS.md's readings of GUIInventory. PlayerItemList binds its list to
// GUIInventory.PartyBackpackView, or to GUIInventory.PartyPlotItemsView on the plot page; the
// screen's list is not the engine's (dev/ITEMS.md: folded by stacking group, sorted, known
// recipes left out). A row has GroupSize, AbilityInstance and Item, the item's object, whose
// fields read on through the row: Name, Description, Tag, Icon, StackSize, Cost, NewlyAcquired,
// EquippableSlots and more (the full set is a member of Party.Backpack's). What the screen's
// character wears is GUI.SelectedChar.Equipment.<slot>, an item of the same fields, there only
// for a slot that holds one. The counter is Party.BackpackSize of Party.BackpackMaxSize, the
// money Party.Money.Total.
//
// Seen in play 2026-11-26 through the Eluvian server, Steam's game on Windows 11, a new human
// noble in Castle Cousland with the inventory open: PartyBackpackView with two rows, GroupSize 1
// each, Item 246 and 820, and through the rows "Lesser Health Poultice" with StackSize 3 and its
// description, and "Blood Dragon Plate"; PartyPlotItemsView empty; Equipment with members 0, 1,
// 4, 6 and 7 (a longsword, a small shield, studded leather armor, boots and gloves), each with
// its name, description, cost and EquippableSlots (3, 2, 16, 64, 128); BackpackSize 2 of 70 and
// Money.Total 0. The same items read as Party.Backpack.0 and .1 and as Objects.246.Name. Then
// with examples/inventory: at the area's load, the screen not up, Pack's two items, no row in
// either view and the five worn slots; with the screen up the two rows with their descriptions.
#include "dao/Inventory.h"

#include "dao/Data.h"

#include <format>

namespace dao::inventory
{

namespace
{

std::int32_t Int(const std::wstring &path)
{
    return static_cast<std::int32_t>(data::Number(path).value_or(0));
}

Item Read(const std::wstring &item)
{
    return {static_cast<ObjectId>(data::Number(item + L".ID").value_or(kInvalidObject)),
            data::Text(item + L".Name").value_or(L""),
            data::Text(item + L".Description").value_or(L""),
            data::Text(item + L".Tag").value_or(L""),
            data::Text(item + L".Icon").value_or(L""),
            Int(item + L".StackSize"),
            Int(item + L".Cost"),
            Int(item + L".NewlyAcquired") != 0,
            static_cast<std::uint32_t>(data::Number(item + L".EquippableSlots").value_or(0))};
}

std::vector<Row> Rows(std::wstring_view view)
{
    std::vector<Row> rows;
    for (const std::wstring &row : data::Members(view))
    {
        rows.push_back({Read(row + L".Item"), Int(row + L".GroupSize")});
    }
    return rows;
}

} // namespace

std::vector<Item> Pack()
{
    std::vector<Item> items;
    for (const std::wstring &item : data::Members(L"Party.Backpack"))
    {
        items.push_back(Read(item));
    }
    return items;
}

std::vector<Row> Backpack()
{
    return Rows(L"GUIInventory.PartyBackpackView");
}

std::vector<Row> PlotItems()
{
    return Rows(L"GUIInventory.PartyPlotItemsView");
}

std::optional<Item> Worn(std::int32_t slot)
{
    const std::wstring item = std::format(L"GUI.SelectedChar.Equipment.{}", slot);
    if (data::Find(item + L".ID") == nullptr)
    {
        return std::nullopt;
    }
    return Read(item);
}

Room PackRoom()
{
    return {Int(L"Party.BackpackSize"), Int(L"Party.BackpackMaxSize")};
}

std::int32_t Money()
{
    return Int(L"Party.Money.Total");
}

} // namespace dao::inventory
