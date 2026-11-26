// Example: the game's message boxes (dao/Gui.h's CurrentPopup, SetPopupInput).
//
// Each message box is logged as it comes up: its text, its buttons with what each sends, and the
// text in its field where it asks for one. With a line of text in a file named
// example_popup.input beside the logs, that text is put in the field of each box that has one,
// and the log has the field as it then reads; the button is left to the player. What it logs goes
// to example_popup.log.
#include "dao/Frame.h"
#include "dao/Gui.h"
#include "dao/Log.h"
#include "dao/Plugin.h"
#include "dao/String.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <string>

namespace
{

bool g_up = false;   // a box was up at the last frame
std::string g_input; // the file's line

void Frame()
{
    const auto box = dao::gui::CurrentPopup();
    if (!box)
    {
        if (g_up)
        {
            spdlog::info("the box is gone");
        }
        g_up = false;
        return;
    }
    if (g_up)
    {
        return;
    }
    g_up = true;
    spdlog::info("a box: {}", dao::Utf8(box->text));
    for (const auto &button : box->buttons)
    {
        spdlog::info("  button {}: sends {}", dao::Utf8(button.label), dao::Utf8(button.callback));
    }
    if (!box->input)
    {
        spdlog::info("  no field");
        return;
    }
    spdlog::info("  its field reads: {}", dao::Utf8(*box->input));
    if (!g_input.empty())
    {
        const bool set = dao::gui::SetPopupInput(dao::Wide(g_input));
        const auto after = dao::gui::CurrentPopup();
        spdlog::info("  {} put in: {}; the field now reads: {}", g_input, set,
                     after && after->input ? dao::Utf8(*after->input) : "nothing");
    }
}

} // namespace

extern "C" __declspec(dllexport) bool __cdecl DAOLib_Load(const dao::LoaderInfo *info)
{
    dao::OpenLog("example_popup");
    if (info == nullptr || info->apiVersion != dao::kPluginApiVersion || !info->knownExecutable)
    {
        spdlog::error("not the loader or the game this was built for");
        return false;
    }
    std::ifstream file(dao::LogDirectory() / L"example_popup.input");
    std::getline(file, g_input);
    dao::OnFrame(&Frame);
    return true;
}
