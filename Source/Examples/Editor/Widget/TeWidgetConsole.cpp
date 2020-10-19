#include "TeWidgetConsole.h"

#include "Math/TeMath.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace te
{
    WidgetConsole::WidgetConsole()
        : Widget(WidgetType::Console)
    { 
        _title = CONSOLE_TITLE;
    }

    WidgetConsole::~WidgetConsole()
    { }

    void WidgetConsole::Initialize()
    { }

    void WidgetConsole::Update()
    { }
}
