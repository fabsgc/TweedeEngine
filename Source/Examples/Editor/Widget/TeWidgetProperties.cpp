#include "TeWidgetProperties.h"

#include "Resources/TeResourceManager.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCScript.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace te
{
    WidgetProperties::WidgetProperties()
        : Widget(WidgetType::Properties)
        , _selections(Editor::Instance().GetSelectionData())
    { 
        _title = PROPERTIES_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetProperties::~WidgetProperties()
    { }

    void WidgetProperties::Initialize()
    { }

    void WidgetProperties::Update()
    {
        bool hasChanged = false;

        if (_selections.ClickedComponent)
        {
            SPtr<GameObject> gameObject = std::static_pointer_cast<GameObject>(_selections.ClickedComponent);
            if (ShowGameObjectInformation(gameObject))
                hasChanged = true;

            switch (_selections.ClickedComponent->GetCoreType())
            {
            case TID_CCamera:
            {
                if (ShowCCameraProperties())
                    hasChanged = true;
            }
            break;

            case TID_CRenderable:
            {
                if (ShowCRenderableProperties())
                    hasChanged = true;
            }
            break;

            case TID_CLight:
            {
                if (ShowCLightProperties())
                    hasChanged = true;
            }
            break;

            case TID_CSkybox:
            {
                if (ShowCSkyboxProperties())
                    hasChanged = true;
            }
            break;

            case TID_CCameraFlyer:
            {
                if (ShowCCameraFlyerProperties())
                    hasChanged = true;
            }
            break;

            case TID_CCameraUI:
            {
                if (ShowCCameraUIProperties())
                    hasChanged = true;
            }
            break;

            case TID_CScript:
            {
                if (ShowCScriptProperties())
                    hasChanged = true;
            }
            break;

            default:
                break;
            }
        }
        else if (_selections.ClickedSceneObject)
        {
            if (ShowSceneObjectProperties())
                hasChanged = true;
        }

        if (hasChanged)
            Editor::Instance().NeedsRedraw();
    }

    void WidgetProperties::UpdateBackground()
    { }

    bool WidgetProperties::ShowSceneObjectProperties()
    {
        bool hasChanged = false;
        ObjectMobility mobility = _selections.ClickedSceneObject->GetMobility();
        Transform transform = _selections.ClickedSceneObject->GetLocalTransform();
        SPtr<GameObject> gameObject = std::static_pointer_cast<GameObject>(_selections.ClickedSceneObject);

        if (ShowGameObjectInformation(gameObject))
            hasChanged = true;

        if (ShowTransform(transform, mobility))
        {
            _selections.ClickedSceneObject->SetLocalTransform(transform);
            _selections.ClickedSceneObject->SetMobility(mobility);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraProperties()
    {
        bool hasChanged = false;
        SPtr<CCamera> camera = std::static_pointer_cast<CCamera>(_selections.ClickedComponent);
        ObjectMobility mobility = camera->_getCamera()->GetMobility();
        Transform transform = camera->_getCamera()->GetTransform();

        if (ShowTransform(transform, mobility))
        {
            //camera->_getCamera()->SetTransform(transform);
            camera->_getCamera()->SetMobility(mobility);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Rendering Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraFlyerProperties()
    {
        bool hasChanged = false;
        SPtr<CCameraFlyer> camera = std::static_pointer_cast<CCameraFlyer>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Flying Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraUIProperties()
    {
        bool hasChanged = false;
        SPtr<CCameraUI> camera = std::static_pointer_cast<CCameraUI>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Orbital Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCLightProperties()
    {
        bool hasChanged = false;
        SPtr<CLight> light = std::static_pointer_cast<CLight>(_selections.ClickedComponent);
        ObjectMobility mobility = light->_getLight()->GetMobility();
        Transform transform = light->_getLight()->GetTransform();

        if (ShowTransform(transform, mobility))
        {
            //light->_getLight()->SetTransform(transform);
            light->_getLight()->SetMobility(mobility);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCRenderableProperties()
    {
        bool hasChanged = false;
        SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(_selections.ClickedComponent);
        ObjectMobility mobility = renderable->_getInternal()->GetMobility();
        Transform transform = renderable->_getInternal()->GetTransform();

        if (ShowTransform(transform, mobility))
        {
            //renderable->_getInternal()->SetTransform(transform);
            renderable->_getInternal()->SetMobility(mobility);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Renderable", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCScriptProperties()
    {
        bool hasChanged = false;
        SPtr<CScript> camera = std::static_pointer_cast<CScript>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCSkyboxProperties()
    {
        bool hasChanged = false;
        SPtr<CSkybox> skybox = std::static_pointer_cast<CSkybox>(_selections.ClickedComponent);
        ObjectMobility mobility = skybox->_getSkybox()->GetMobility();
        Transform transform = skybox->_getSkybox()->GetTransform();

        if (ShowTransform(transform, mobility))
        {
            //skybox->_getSkybox()->SetTransform(transform);
            skybox->_getSkybox()->SetMobility(mobility);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Skybox", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        return hasChanged;
    }

    bool WidgetProperties::ShowTransform(Transform& transform, ObjectMobility& mobility)
    {
        bool hasChanged = false;
        const float widgetWidth = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Vector<int> mobilityOptions = {
                (int)ObjectMobility::Movable,
                (int)ObjectMobility::Immovable,
                (int)ObjectMobility::Static
            };

            Vector<String> mobilityLabels = {
                "Movable", "Immovable", "Static"
            };

            if (ImGuiExt::RenderOptionCombo((int*)(&mobility), "##transform_mobility_option", "Mobility", mobilityOptions, mobilityLabels, widgetWidth))
                hasChanged = true;

            if (ImGuiExt::RenderTransform(transform, "transform_option"))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowGameObjectInformation(SPtr<GameObject>& gameObject)
    {
        char inputName[128];
        char inputUUID[64];
        bool hasChanged = false;
        String name = gameObject->GetName();
        String uuid = gameObject->GetUUID().ToString();
        const float widgetWidth = ImGui::GetWindowContentRegionWidth() - 100.0f;

        strcpy(inputName, name.c_str());
        strcpy(inputUUID, uuid.c_str());

        if (ImGui::CollapsingHeader("Identification", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(widgetWidth);
            if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
            {
                gameObject->SetName(inputName);
                hasChanged = true;
            }

            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            if (ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID)))
            { }
            ImGui::PopItemFlag();
            ImGui::PopItemWidth();
        }

        return hasChanged;
    }
}
