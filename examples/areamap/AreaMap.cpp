// Example: the area map's notes (dao/AreaMap.h).
//
// Each time an area has loaded, and each time the map opens, the area's name and North are
// logged, then every note with its text, its picture, its place and whether the map draws it,
// and the places the active quests point to. What it logs goes to example_areamap.log.
#include "dao/AreaMap.h"
#include "dao/GameModes.h"
#include "dao/Gui.h"
#include "dao/Log.h"
#include "dao/Plugin.h"
#include "dao/String.h"

#include <spdlog/spdlog.h>

#include <string_view>

namespace
{

namespace map = dao::areamap;

void LogMap(std::string_view when)
{
    spdlog::info("{}: {}, North {}", when, dao::Utf8(map::AreaName()), map::North());
    for (const map::Note &note : map::Notes())
    {
        spdlog::info("note {} ({}, {}) at {:.1f}, {:.1f}{}", dao::Utf8(note.text), dao::Utf8(note.tag),
                     dao::Utf8(note.icon), note.x, note.y, note.visible ? "" : ", not drawn");
    }
    for (const map::Destination &destination : map::Destinations())
    {
        spdlog::info("quest {} points to 0x{:x}", dao::Utf8(destination.quest), destination.object);
    }
}

} // namespace

extern "C" __declspec(dllexport) bool __cdecl DAOLib_Load(const dao::LoaderInfo *info)
{
    dao::OpenLog("example_areamap");
    if (info == nullptr || info->apiVersion != dao::kPluginApiVersion || !info->knownExecutable)
    {
        spdlog::error("not the loader or the game this was built for");
        return false;
    }
    dao::modes::OnAreaLoaded([] { LogMap("an area has loaded"); });
    dao::gui::OnFullScreen([](std::wstring_view name, bool shown) {
        if (shown && name == L"Map")
        {
            LogMap("the map is open");
        }
    });
    return true;
}
