// The tutorial box and the training step (dao/Tutorials.h).
//
// Notes on the tutorials: read 2026-11-26 in popuplayer.gfx as the game takes it
// (packages/core/patch/patch003.erf, decompiled with FFDec 26.3.0), with dev/PLOTS.md's reading
// of the tutorial control. The box is PopupLayer's: HandleTutorialUpdate, on the tokens
// Tutorials.TutorialList and Tutorials.Enabled, shows it while the list has a member and Enabled
// is not 0, with the list's member 0: its Title, its Text and its CodexResRef, which the box's
// book button sends as "GUI.GoToCodex <resref>". With one member the box has its Close button,
// with more its Next; Close sends "Tutorials.ConsumeTutorial 1" and Next "Tutorials.ConsumeTutorial
// 0". The box also has a Show Tutorials toggle, which sets ClientOptions.GameOptions.ShowTutorials
// and sends "ClientOptions.CommitOptions".
//
// The training step is the token Tutorials.TrainingStep: UpdateTrainingText shows, while Enabled
// is not 0 and the step is not 0, the step's row of table 287 (TrainingTutorials, the worksheets
// trainingsteps*): the talk-table string of its Title (the movie's _2DA.HASH_TITLE, 709193354)
// and of its StrRef (HASH_STRREF, 1080246718), the latter with its designer tags, such as
// <ZoomInMouse/>, filled in.
//
// Seen in play 2026-11-26 through the Eluvian server, Steam's game on Windows 11, a new human
// noble's start in Castle Cousland: TutorialList with one member, "Fatigue" with its text and
// tut_fatigue, and after "Tutorials.ConsumeTutorial 1" none; later one more, "Plot Helpers",
// gone the same way. TrainingStep 1 from the area's load, the game paused, SaveLoad.SavingEnabled
// 0 and a save refused; the step 2 after the player's CameraZoomInMessage with the camera at its
// closest; unchanged by an order to move given by command and by
// PerformPrimaryActionOnTargetUnderCursorMessage over walkable ground; 9 after the script
// command BeginTrainingStep(9, 1069, 0); and 0, the game running and saving allowed, after the
// player's UserTogglePauseMessage. Then with examples/tutorials through the same start: step 1
// logged as "Tutorial: Tactical View" with its text and "(Scroll forward)" where the tag was,
// "Fatigue" logged with one waiting and put away by Dismiss, step 2 ("Right-click on your desired
// destination"), step 9 ("Press Space to unpause now"), and none. With a save loaded and the map
// and the inventory opened, "Area Map" came with one waiting and "Inventory" with two, the newer
// one in the box.
#include "dao/Tutorials.h"

#include "dao/Data.h"
#include "dao/Gda.h"
#include "dao/Messages.h"
#include "dao/Tlk.h"

namespace dao::tutorials
{

namespace
{

constexpr std::int32_t kTrainingSteps = 287;  // the table TrainingTutorials
constexpr std::uint32_t kTitle = 709193354;   // its column Title, a string id: the movie's _2DA.HASH_TITLE
constexpr std::uint32_t kStrRef = 1080246718; // its column StrRef, a string id: _2DA.HASH_STRREF

bool Enabled()
{
    return data::Number(L"Tutorials.Enabled").value_or(0) != 0;
}

std::wstring TextOf(std::int32_t step, std::uint32_t column)
{
    const auto strref = gda::GetInt(kTrainingSteps, step, column);
    return strref ? tlk::Resolve(tlk::String(static_cast<std::uint32_t>(*strref))) : std::wstring{};
}

} // namespace

std::size_t Waiting()
{
    return Enabled() ? data::MemberCount(L"Tutorials.TutorialList").value_or(0) : 0;
}

std::optional<Tutorial> Current()
{
    if (Waiting() == 0)
    {
        return std::nullopt;
    }
    return Tutorial{data::Text(L"Tutorials.TutorialList.0.Title").value_or(L""),
                    data::Text(L"Tutorials.TutorialList.0.Text").value_or(L""),
                    data::Text(L"Tutorials.TutorialList.0.CodexResRef").value_or(L"")};
}

bool Dismiss()
{
    const std::size_t waiting = Waiting();
    if (waiting == 0)
    {
        return false;
    }
    return messages::SendText(waiting == 1 ? L"Tutorials.ConsumeTutorial 1" : L"Tutorials.ConsumeTutorial 0");
}

std::optional<Step> TrainingStep()
{
    const auto id = static_cast<std::int32_t>(data::Number(L"Tutorials.TrainingStep").value_or(0));
    if (!Enabled() || id == 0)
    {
        return std::nullopt;
    }
    return Step{id, TextOf(id, kTitle), TextOf(id, kStrRef)};
}

} // namespace dao::tutorials
