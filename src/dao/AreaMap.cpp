// The area map's notes (dao/AreaMap.h).
//
// Notes on the area map: read 2026-11-26 in map.gfx as the game takes it
// (packages/core/patch/patch003.erf, decompiled with FFDec 26.3.0; SharedLibrary.SharedMap,
// MapNoteIcon and PlotMapIcon) and in dev/gui_data_paths.csv's rows for the movie. The map reads
// Area.Name for its title and Area.North for its turn: WorldToMapCoordinates puts a world point
// at ((x - left) / width, 1 - (y - bottom) / height) of the picture, +x to the right and +y up,
// and turns that by North quarter turns, the picture's _rotation 0, 90, 180 or 270 clockwise.
// The scripts have no command for a note's text (dev/script_commands/waypatches.md has the pins'
// state alone), so the text is here or nowhere: each member of MapNoteList.MapNotes has MapNote
// (the text), Icon, MapNoteVisible, Tag and WorldPositionX and WorldPositionY, and each member of
// Journal.PlotDestinations an Object, a Tooltip (the quest's name) and ActiveQuest.
//
// Seen in play 2026-11-26 through the Eluvian server, Steam's game on Windows 11, a new game in
// Castle Cousland with the map open: Area.Name "Castle Cousland", Area.North 0, ten notes each
// with its text ("Main Hall", "Chapel", "Your Room" ...), ico_mm_mapPin, MapNoteVisible 1, its
// tag and its place, and one destination, Object 690 with the quest's name. The ten are the
// area file's ten waypoints with HasMapNote, the same texts at the same places (read with
// Eluvian's ghilana). Then with examples/areamap: the same ten notes and the destination at the
// area's load, the map not up, each note's MapNoteVisible 0, and with the map open each 1. An area
// with North other than 0 was not seen, nor a note the map keeps hidden while it is up.
#include "dao/AreaMap.h"

#include "dao/Data.h"

namespace dao::areamap
{

std::wstring AreaName()
{
    return data::Text(L"Area.Name").value_or(L"");
}

std::int32_t North()
{
    return static_cast<std::int32_t>(data::Number(L"Area.North").value_or(0));
}

std::vector<Note> Notes()
{
    std::vector<Note> notes;
    for (const std::wstring &member : data::Members(L"MapNoteList.MapNotes"))
    {
        notes.push_back({data::Text(member + L".MapNote").value_or(L""), data::Text(member + L".Icon").value_or(L""),
                         data::Text(member + L".Tag").value_or(L""),
                         data::Number(member + L".MapNoteVisible").value_or(0) != 0,
                         static_cast<float>(data::Number(member + L".WorldPositionX").value_or(0)),
                         static_cast<float>(data::Number(member + L".WorldPositionY").value_or(0))});
    }
    return notes;
}

std::vector<Destination> Destinations()
{
    std::vector<Destination> destinations;
    for (const std::wstring &member : data::Members(L"Journal.PlotDestinations"))
    {
        destinations.push_back({static_cast<ObjectId>(data::Number(member + L".Object").value_or(0)),
                                data::Text(member + L".Tooltip").value_or(L"")});
    }
    return destinations;
}

} // namespace dao::areamap
