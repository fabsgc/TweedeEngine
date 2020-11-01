#include "TeWidgetRenderOptions.h"
#include "TeWidgetViewport.h"

#include "Renderer/TeRenderSettings.h"
#include "Renderer/TeCamera.h"

#include "Components/TeCCamera.h"

#include "../TeEditor.h"

namespace te
{
    WidgetRenderOptions::WidgetRenderOptions()
        : Widget(WidgetType::RenderOptions)
    {
        _title = RENDER_OPTIONS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetRenderOptions::~WidgetRenderOptions()
    { }

    void WidgetRenderOptions::Initialize()
    { }

    void WidgetRenderOptions::Update()
    {
        bool hasChanged = false;
        HCamera& camera = gEditor().GetViewportCamera();
        auto cameraSettings = camera->GetRenderSettings();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGuiExt::RenderCameraGraphics(camera, cameraSettings, width))
                hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Post processing", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGuiExt::RenderCameraPostProcessing(camera, cameraSettings, width))
                hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        if (hasChanged)
        {
            gEditor().NeedsRedraw();
            camera->SetRenderSettings(cameraSettings);
        }
    }

    void WidgetRenderOptions::UpdateBackground()
    { }
}
