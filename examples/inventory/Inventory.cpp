// Example: the inventory's lists (dao/Inventory.h).
//
// Each time an area has loaded, and each time the inventory opens, the party's pack is logged
// row by row as the screen lists it, then the plot items, what the screen's character wears in
// each slot, the pack's room and the party's money. What it logs goes to example_inventory.log.
#include "dao/Inventory.h"
#include "dao/GameModes.h"
#include "dao/Gui.h"
#include "dao/Log.h"
#include "dao/Plugin.h"
#include "dao/String.h"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace
{

namespace inventory = dao::inventory;

constexpr std::int32_t kSlots = 20; // past the last INVENTORY_SLOT_* a creature has

void LogRows(std::string_view list, const std::vector<inventory::Row> &rows)
{
    spdlog::info("{}: {} rows", list, rows.size());
    for (const inventory::Row &row : rows)
    {
        spdlog::info("  {} x{} (0x{:x}, {}), cost {}, a row of {}{}: {}", dao::Utf8(row.item.name), row.item.stack,
                     row.item.id, dao::Utf8(row.item.tag), row.item.cost, row.groupSize, row.item.isNew ? ", new" : "",
                     dao::Utf8(row.item.description));
    }
}

void LogInventory(std::string_view when)
{
    const inventory::Room room = inventory::PackRoom();
    spdlog::info("{}: the pack holds {} of {}, the party has {} copper", when, room.used, room.most,
                 inventory::Money());
    for (const inventory::Item &item : inventory::Pack())
    {
        spdlog::info("in the pack: {} x{} (0x{:x})", dao::Utf8(item.name), item.stack, item.id);
    }
    LogRows("the pack as the screen lists it", inventory::Backpack());
    LogRows("plot items", inventory::PlotItems());
    for (std::int32_t slot = 0; slot < kSlots; ++slot)
    {
        if (const auto item = inventory::Worn(slot))
        {
            spdlog::info("slot {}: {} (0x{:x}), fits slots 0x{:x}", slot, dao::Utf8(item->name), item->id,
                         item->equippableSlots);
        }
    }
}

} // namespace

extern "C" __declspec(dllexport) bool __cdecl DAOLib_Load(const dao::LoaderInfo *info)
{
    dao::OpenLog("example_inventory");
    if (info == nullptr || info->apiVersion != dao::kPluginApiVersion || !info->knownExecutable)
    {
        spdlog::error("not the loader or the game this was built for");
        return false;
    }
    dao::modes::OnAreaLoaded([] { LogInventory("an area has loaded"); });
    dao::gui::OnFullScreen([](std::wstring_view name, bool shown) {
        if (shown && name == L"Inventory")
        {
            LogInventory("the inventory is open");
        }
    });
    return true;
}
