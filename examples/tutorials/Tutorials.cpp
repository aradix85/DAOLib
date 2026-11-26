// Example: the tutorial box and the training step (dao/Tutorials.h).
//
// Each tutorial is logged as it comes into the box, with how many wait, and each training step as
// the game sets it, with what it asks of the player. With an empty file named
// example_tutorials.dismiss beside the logs, each tutorial is also put away as its button does,
// so a start with nobody at the mouse is not left behind a box. What it logs goes to
// example_tutorials.log.
#include "dao/Tutorials.h"
#include "dao/Frame.h"
#include "dao/Log.h"
#include "dao/Plugin.h"
#include "dao/String.h"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <string>
#include <system_error>

namespace
{

namespace tutorials = dao::tutorials;

std::wstring g_title; // the tutorial last logged
std::int32_t g_step = 0;
bool g_dismiss = false;

void Frame()
{
    const auto step = tutorials::TrainingStep();
    const std::int32_t id = step ? step->id : 0;
    if (id != g_step)
    {
        g_step = id;
        if (step)
        {
            spdlog::info("training step {}: {}: {}", step->id, dao::Utf8(step->title), dao::Utf8(step->text));
        }
        else
        {
            spdlog::info("no training step");
        }
    }
    const auto tutorial = tutorials::Current();
    if (!tutorial)
    {
        g_title.clear();
        return;
    }
    if (tutorial->title == g_title)
    {
        return;
    }
    g_title = tutorial->title;
    spdlog::info("tutorial, {} waiting: {} ({}): {}", tutorials::Waiting(), dao::Utf8(tutorial->title),
                 dao::Utf8(tutorial->codex), dao::Utf8(tutorial->text));
    if (g_dismiss)
    {
        spdlog::info("put away: {}", tutorials::Dismiss());
    }
}

} // namespace

extern "C" __declspec(dllexport) bool __cdecl DAOLib_Load(const dao::LoaderInfo *info)
{
    dao::OpenLog("example_tutorials");
    if (info == nullptr || info->apiVersion != dao::kPluginApiVersion || !info->knownExecutable)
    {
        spdlog::error("not the loader or the game this was built for");
        return false;
    }
    std::error_code ec;
    g_dismiss = std::filesystem::exists(dao::LogDirectory() / L"example_tutorials.dismiss", ec);
    dao::OnFrame(&Frame);
    return true;
}
