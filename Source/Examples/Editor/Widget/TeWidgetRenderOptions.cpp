#include "TeWidgetRenderOptions.h"
#include "TeWidgetViewport.h"

#include "Renderer/TeRenderer.h"
#include "Manager/TeRendererManager.h"
#include "Scene/TeSceneManager.h"

#include "Renderer/TeRenderSettings.h"
#include "Renderer/TeCamera.h"

#include "Components/TeCCamera.h"

#include "../TeEditor.h"

namespace te
{
    WidgetRenderOptions::WidgetRenderOptions()
        : Widget(WidgetType::RenderOptions)
        , _renderer(RendererManager::Instance().GetRenderer())
    {
        _title = RENDER_OPTIONS_TITLE;
    }

    WidgetRenderOptions::~WidgetRenderOptions()
    { }

    void WidgetRenderOptions::Initialize()
    { }

    void WidgetRenderOptions::Update()
    {
        bool hasChanged = false;
        bool anotherCameraSelected = false;
        WidgetViewport* widgetViewport = static_cast<WidgetViewport*>(Editor::Instance().GetWidget(WidgetType::Viewport));

        HCamera camera = widgetViewport->GetSceneCamera();
        HCamera viewportCamera = widgetViewport->GetSceneCamera();
        
        SPtr<RenderSettings> cameraSettings = camera->GetRenderSettings();
        SPtr<RenderSettings> viewportSettings = viewportCamera->GetRenderSettings();

        //If a camera is selected, we display its settings
        //else, we display viewport camera settings

        if (!anotherCameraSelected)
            widgetViewport->ResetCameraSettings();

        // Show UI
        const auto renderOptionFloat = [&](
            float& value,
            const char* id, 
            const char* text,
            float min = 0.0f, 
            float max = std::numeric_limits<float>::max())
        {
            const float previousValue = value;

            ImGui::PushID(id);
            ImGui::SliderFloat(text, &value, min, max, "%.2f");
            ImGui::PopID();
            value = Math::Clamp(value, min, max);

            if (previousValue != value)
                hasChanged = true;
        };

        const auto renderOptionInt = [&](
            int& value,
            const char* id,
            const char* text,
            int min = 0.0f,
            int max = std::numeric_limits<int>::max())
        {
            const int previousValue = value;

            ImGui::PushID(id);
            ImGui::SliderInt(text, &value, min, max, "%.d");
            ImGui::PopID();
            value = Math::Clamp(value, min, max);

            if (previousValue != value)
                hasChanged = true;
        };

        const auto renderOptionBool = [&](
            bool& value,
            const char* id,
            const char* text
            )
        {
            const bool previousValue = value;

            ImGui::PushID(id);
            ImGui::Checkbox(text, &value);
            ImGui::PopID();

            if (previousValue != value)
                hasChanged = true;
        };

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            renderOptionBool(cameraSettings->EnableHDR, "##hdr_option", "Enable HDR");
            renderOptionBool(cameraSettings->Tonemapping.Enabled, "##tonemapping_option", "Enable Tonemapping");
            renderOptionFloat(cameraSettings->ExposureScale, "##exposure_option", "Exposure", 0.0f, 5.0f);
            renderOptionFloat(cameraSettings->Gamma, "##gamma_option", "Gamma", 0.0f, 5.0f);
            renderOptionFloat(cameraSettings->Contrast, "##contrast_option", "Contrast", 0.0f, 5.0f);
            renderOptionFloat(cameraSettings->Brightness, "##brightness_option", "Brightness", -2.0f, 2.0f);
            
            ImGui::Separator();
            
            Vector<UINT32> antialiasingOptions = { 
                (UINT32)AntiAliasingAlgorithm::FXAA,
                (UINT32)AntiAliasingAlgorithm::TAA,
                (UINT32)AntiAliasingAlgorithm::None
            };

            Vector<String> antialiasingLabels = {
                "FXAA", "TAA", "None"
            };

            if (ImGui::BeginCombo("Antialiasing", antialiasingLabels[(UINT32)cameraSettings->AntialiasingAglorithm].c_str()))
            {
                ImGui::EndCombo();
            }
            
            ImGui::Separator();
        }

        if (hasChanged)
        {
            if (anotherCameraSelected) // another camera is selected
                viewportCamera->SetRenderSettings(cameraSettings);

            Editor::Instance().NeedsRedraw();
        }
    }

    void WidgetRenderOptions::UpdateBackground()
    { }
}
