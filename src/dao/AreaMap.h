// The area map as its screen shows it: the area's name, which way is north, the map's notes with
// their texts and places, and the places the active quests point to.
//
//   for (const dao::areamap::Note &note : dao::areamap::Notes())
//       if (note.visible)
//           log(note.text, note.x, note.y);
#pragma once

#include "dao/ObjectId.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dao::areamap
{

// Returns the area's name as the map's title shows it, in the player's language.
//
// Call it, and the rest, on the game thread, with a game in play.
std::wstring AreaName();

// Returns the area's North, 0 to 3: how many quarter turns clockwise the map turns its picture,
// which has the world's +x to the right and +y up. So north is the world's +y for 0, -x for 1, -y
// for 2 and +x for 3, and east a quarter turn clockwise from north.
std::int32_t North();

// A note on the map: a pin, an exit, a merchant.
struct Note
{
    std::wstring text;    // as the map shows it under the cursor: L"Main Hall"
    std::wstring icon;    // the note's picture, which tells its kind: L"ico_mm_mapPin"
    std::wstring tag;     // the tag of the waypoint that carries the note
    bool visible = false; // the map draws it; false for every note while the map is not up
    float x = 0;          // its place in the world, in metres
    float y = 0;
};

// Returns the area's notes, in the map's own order.
std::vector<Note> Notes();

// A place an active quest points to.
struct Destination
{
    ObjectId object = 0; // the object the map's marker stands on
    std::wstring quest;  // the quest's name, as the marker's tooltip shows it
};

// Returns the places the journal's active quests point to in this area.
std::vector<Destination> Destinations();

} // namespace dao::areamap
