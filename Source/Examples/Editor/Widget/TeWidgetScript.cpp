#include "TeWidgetScript.h"
#include "../TeEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "TeCoreApplication.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    WidgetScript::WidgetScript()
        : Widget(WidgetType::Script)
    {
        _title = SCRIPT_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove;
    }

    WidgetScript::~WidgetScript()
    { }

    void WidgetScript::Initialize()
    { }

    void WidgetScript::Update()
    { }

    void WidgetScript::UpdateBackground()
    { }
}
