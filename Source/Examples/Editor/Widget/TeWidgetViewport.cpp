#include "TeWidgetViewport.h"

#include "ImGui/imgui.h"

namespace te
{
    WidgetViewport::WidgetViewport()
        : Widget(WidgetType::Viewport)
    { 
        _title = "Viewport";
        _size = Vector2(400, 250);
        _flags |= ImGuiWindowFlags_NoScrollbar;
        _padding = Vector2(4.0f, 4.0f);
    }

    WidgetViewport::~WidgetViewport()
    { }

    void WidgetViewport::Initialize()
    { }

    void WidgetViewport::Update()
    { 
        // Get current frame window resolution
        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
    }
}
