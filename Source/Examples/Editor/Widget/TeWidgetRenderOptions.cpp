#include "TeWidgetRenderOptions.h"

#include "Renderer/TeRenderSettings.h"
#include "Components/TeCCamera.h"
#include "../TeEditor.h"

using namespace std::placeholders;

namespace te
{
    WidgetRenderOptions::WidgetRenderOptions()
        : Widget(WidgetType::RenderOptions)
        , _currentCamera(gEditor().GetViewportCamera())
    {
        _title = RENDER_OPTIONS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;

        _cameraList.AddOption(_currentCamera, _currentCamera->GetName());

        _cameraCreated = Component::OnComponentCreated.Connect(std::bind(&WidgetRenderOptions::CameraCreated, this, _1));
        _cameraDestroyed = Component::OnComponentDestroyed.Connect(std::bind(&WidgetRenderOptions::CameraDestroyed, this, _1));
    }

    WidgetRenderOptions::~WidgetRenderOptions()
    { 
        _cameraCreated.Disconnect();
        _cameraDestroyed.Disconnect();
    }

    void WidgetRenderOptions::Initialize()
    { }

    void WidgetRenderOptions::Update()
    {
        bool hasChanged = false;
        auto cameraSettings = _currentCamera->GetRenderSettings();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGuiExt::RenderOptionComboComponent(&_currentCamera, "##renderoptions_switch_camera_option", "", _cameraList, ImGui::GetWindowContentRegionWidth()))
        {
            gEditor().SetPreviewViewportCamera(_currentCamera);
            cameraSettings = _currentCamera->GetRenderSettings();
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGuiExt::RenderCameraGraphics(_currentCamera, cameraSettings, width))
                hasChanged = true;
        }

        if (ImGuiExt::RenderCameraPostProcessing(_currentCamera, cameraSettings, width))
            hasChanged = true;

        if (hasChanged)
        {
            gEditor().NeedsRedraw();
            _currentCamera->SetRenderSettings(cameraSettings);
        }
    }

    void WidgetRenderOptions::UpdateBackground()
    { }

    void WidgetRenderOptions::CameraCreated(const HComponent& component)
    {
        if (component->GetCoreType() != TID_CCamera)
            return;

        HCamera camera = static_object_cast<CCamera>(component);
        _cameraList.AddOption(camera, camera->GetName());
    }

    void WidgetRenderOptions::CameraDestroyed(const HComponent& component)
    {
        if (component->GetCoreType() != TID_CCamera)
            return;

        for (auto it = _cameraList.Options.begin(); it != _cameraList.Options.end(); it++)
        {
            if (component == _currentCamera)
            {
                _currentCamera = _cameraList.Options[0].Key.GetNewHandleFromExisting();
                gEditor().SetPreviewViewportCamera(_currentCamera);
                gEditor().NeedsRedraw();
            }

            if (it->Key == component)
            {
                _cameraList.Options.erase(it);
                break;
            }
        }
    }
}
