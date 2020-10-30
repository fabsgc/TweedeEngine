#include "TeWidgetMaterials.h"
#include "../TeEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "TeCoreApplication.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    WidgetMaterials::WidgetMaterials()
        : Widget(WidgetType::Game)
    {
        _title = MATERIALS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetMaterials::~WidgetMaterials()
    { }

    void WidgetMaterials::Initialize()
    { }

    void WidgetMaterials::Update()
    { }

    void WidgetMaterials::UpdateBackground()
    { }
}
