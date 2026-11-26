// The game's tutorials: the tutorial box that is up, with its text and its button, and the step of
// a training session, which holds the game paused until the player has done what it asks.
//
//   if (const auto tutorial = dao::tutorials::Current())
//   {
//       log(tutorial->title, tutorial->text);
//       dao::tutorials::Dismiss();
//   }
//   if (const auto step = dao::tutorials::TrainingStep())
//       log(step->title, step->text); // "Use the mouse wheel to bring your camera down ..."
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace dao::tutorials
{

// A tutorial as its box shows it.
struct Tutorial
{
    std::wstring title;
    std::wstring text;
    std::wstring codex; // the codex entry the box's book button opens: L"tut_fatigue"
};

// Returns how many tutorials wait to be shown, the one in the box among them.
// @return 0 when none waits, or the player's options have tutorials off
//
// Call it, and the rest, on the game thread.
std::size_t Waiting();

// Returns the tutorial in the box.
// @return nullopt when no box is up
std::optional<Tutorial> Current();

// Puts the tutorial in the box away as its button does, the next one taking its place: the box
// has Next while more wait and Close for the last, and both do this.
// @return false, with nothing done, when no box is up
bool Dismiss();

// A step of a training session, as the screen's top shows it.
struct Step
{
    std::int32_t id = 0; // a row of the training steps table
    std::wstring title;  // "Tutorial: Tactical View"
    std::wstring text;   // what the player is to do, its keys as the player has them bound
};

// Returns the step a training session waits on. While one is set the game stays paused and
// allows no save, and takes an unpause only where the step asks for it and only from the player's
// own pause key (dev/PLOTS.md).
// @return nullopt when no training session runs, or the player's options have tutorials off
std::optional<Step> TrainingStep();

} // namespace dao::tutorials
