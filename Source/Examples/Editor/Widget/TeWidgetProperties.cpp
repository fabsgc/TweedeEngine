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
#include "RenderAPI/TeSubMesh.h"
#include "Mesh/TeMesh.h"

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

        if (ImGui::CollapsingHeader("Rendering Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowCamera(camera))
                hasChanged = true;
        }

        if (ShowTransform(transform, mobility, true))
        {
            camera->_getCamera()->SetMobility(mobility);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraFlyerProperties()
    {
        bool hasChanged = false;
        SPtr<CCameraFlyer> camera = std::static_pointer_cast<CCameraFlyer>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Flying Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowCameraFlyer(camera))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraUIProperties()
    {
        bool hasChanged = false;
        SPtr<CCameraUI> camera = std::static_pointer_cast<CCameraUI>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Orbital Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowCamerUI(camera))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCLightProperties()
    {
        bool hasChanged = false;
        SPtr<CLight> light = std::static_pointer_cast<CLight>(_selections.ClickedComponent);
        ObjectMobility mobility = light->_getLight()->GetMobility();
        Transform transform = light->_getLight()->GetTransform();

        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowLight(light->_getLight()))
                hasChanged = true;
        }

        if (ShowTransform(transform, mobility, true))
        {
            light->_getLight()->SetMobility(mobility);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCRenderableProperties()
    {
        bool hasChanged = false;
        SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(_selections.ClickedComponent);
        ObjectMobility mobility = renderable->_getInternal()->GetMobility();
        Transform transform = renderable->_getInternal()->GetTransform();

        if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowRenderableProperties(renderable->_getInternal()))
                hasChanged = true;
        }

        if (ShowRenderableSubMeshes(renderable->_getInternal()))
            hasChanged = true;

        if (ShowTransform(transform, mobility, true))
        {
            renderable->_getInternal()->SetMobility(mobility);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCScriptProperties()
    {
        bool hasChanged = false;
        SPtr<CScript> script = std::static_pointer_cast<CScript>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowScript(script))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCSkyboxProperties()
    {
        bool hasChanged = false;
        SPtr<CSkybox> skybox = std::static_pointer_cast<CSkybox>(_selections.ClickedComponent);
        ObjectMobility mobility = skybox->_getSkybox()->GetMobility();
        Transform transform = skybox->_getSkybox()->GetTransform();

        if (ImGui::CollapsingHeader("Skybox", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowSkybox(skybox->_getSkybox()))
                hasChanged = true;
        }

        if (ShowTransform(transform, mobility, true))
        {
            skybox->_getSkybox()->SetMobility(mobility);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowTransform(Transform& transform, ObjectMobility& mobility, bool disableTransform)
    {
        bool hasChanged = false;
        const float widgetWidth = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGuiExt::ComboOptions mobilityOptions;
            mobilityOptions.AddOption((int)ObjectMobility::Movable, "Movable");
            mobilityOptions.AddOption((int)ObjectMobility::Immovable, "Immovable");
            mobilityOptions.AddOption((int)ObjectMobility::Static, "Static");

            if (ImGuiExt::RenderOptionCombo((int*)(&mobility), "##transform_mobility_option", "Mobility", mobilityOptions, widgetWidth))
                hasChanged = true;

            if (ImGuiExt::RenderTransform(transform, "transform_option", disableTransform))
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

    bool WidgetProperties::ShowCamera(SPtr<CCamera> camera)
    {
        bool hasChanged = false;
        HCamera cameraHandle;
        cameraHandle._setHandleData(camera);
        auto& cameraSettings = camera->GetRenderSettings();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGuiExt::RenderCameraGraphics(cameraHandle, cameraSettings, width))
                hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Post processing", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGuiExt::RenderCameraPostProcessing(cameraHandle, cameraSettings, width))
                hasChanged = true;
        }

        if (hasChanged)
        {
            Editor::Instance().NeedsRedraw();
            camera->SetRenderSettings(cameraSettings);
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCameraFlyer(SPtr<CCameraFlyer> cameraFlyer)
    {
        bool hasChanged = false;
        Degree pitch = cameraFlyer->GetPitch();
        Degree yaw = cameraFlyer->GetYaw();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        float pitchAngle = pitch.ValueDegrees();
        float yawAngle = yaw.ValueDegrees();
        ImGuiExt::RenderOptionFloat(pitchAngle, "##camera_fliyer_pitch_option", "Current pitch", -180.0f, 180.0f, width, true);
        ImGuiExt::RenderOptionFloat(yawAngle, "##camera_fliyer_yaw_option", "Current yaw", -180.0f, 180.0f, width, true);

        return hasChanged;
    }

    bool WidgetProperties::ShowCamerUI(SPtr<CCameraUI> cameraUI)
    {
        bool hasChanged = false;
        Vector3 target = cameraUI->GetTarget();

        if (ImGuiExt::RenderVector3(target, "##camera_ui_target_option", " Target", 20.0f, false))
        {
            cameraUI->SetTarget(target);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowLight(SPtr<Light> light)
    {
        bool hasChanged = false;
        int lightType = (int)light->GetType();
        bool castShadows = light->GetCastsShadow();
        Color color = light->GetColor();
        float attenuationRadius = light->GetAttenuationRadius();
        float linearAttenuation = light->GetLinearAttenuation();
        float quadraticAttenuation = light->GetQuadraticAttenuation();
        float intensity = light->GetIntensity();
        Degree spotAngle = light->GetSpotAngle();
        float shadowBias = light->GetShadowBias();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        // Color
        {
            Vector4 color = light->GetColor().GetAsVector4();

            if (ImGuiExt::RenderColorRGBA(color, "##light_color_option", "Color", width))
            {
                hasChanged = true;
                light->SetColor(Color(color));
            }
        }

        // Light Type
        {
            ImGuiExt::ComboOptions lightTypeOptions;
            lightTypeOptions.AddOption((int)LightType::Directional, "Directional");
            lightTypeOptions.AddOption((int)LightType::Radial, "Radial");
            lightTypeOptions.AddOption((int)LightType::Spot, "Spot");

            if (ImGuiExt::RenderOptionCombo((int*)(&lightType), "##light_type_option", "Type", lightTypeOptions, width, true))
            {
                light->SetType((LightType)lightType);
                hasChanged = true;
            }
        }

        // Attenuation radius
        {
            if (ImGuiExt::RenderOptionFloat(attenuationRadius, "##light_attenuation_radius_option", "Att. Radius", 0.0f, 5.0f, width))
            {
                hasChanged = true;
                light->SetAttenuationRadius(attenuationRadius);
            }
        }

        // Linear attenuation
        {
            if (ImGuiExt::RenderOptionFloat(linearAttenuation, "##light_linear_attenuation_option", "Linear att.", 0.0f, 1.0f, width))
            {
                hasChanged = true;
                light->SetLinearAttenuation(linearAttenuation);
            }
        }

        // quadratic attenuation
        {
            if (ImGuiExt::RenderOptionFloat(quadraticAttenuation, "##light_quadratic_attenuation_option", "Quadratic att.", 0.0f, 1.0f, width))
            {
                hasChanged = true;
                light->SetQuadraticAttenuation(quadraticAttenuation);
            }
        }

        // intensity
        {
            if (ImGuiExt::RenderOptionFloat(intensity, "##light_intensity_option", "Intensity", 0.0f, 5.0f, width))
            {
                hasChanged = true;
                light->SetIntensity(intensity);
            }
        }

        // Spot angle
        if (light->GetType() == LightType::Spot)
        {
            {
                float angle = spotAngle.ValueDegrees();
                if (ImGuiExt::RenderOptionFloat(angle, "##light_spot_angle_option", "Spot angle", 0.0f, 179.0f, width))
                {
                    hasChanged = true;
                    light->SetSpotAngle(Degree(angle));
                }
            }
        }

        // Shadow bias
        {
            if (ImGuiExt::RenderOptionFloat(shadowBias, "##light_shadow_bias_option", "Shadow bias", 0.0f, 5.0f, width))
            {
                hasChanged = true;
                light->SetShadowBias(shadowBias);
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowRenderableProperties(SPtr<Renderable> renderable)
    {
        bool hasChanged = false;
        const RenderableProperties& properties = renderable->GetProperties();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        // instancing
        {
            bool instancing = properties.Instancing;
            if (ImGuiExt::RenderOptionBool(instancing, "##renderable_properties_instancing_option", "Instancing"))
            {
                hasChanged = true;
                renderable->SetInstancing(instancing);
            }
        }

        // can be merged
        {
            bool canBeMerged = properties.CanBeMerged;
            if (ImGuiExt::RenderOptionBool(canBeMerged, "##renderable_properties_can_be_merged_option", "Merging"))
            {
                hasChanged = true;
                renderable->SetCanBeMerged(canBeMerged);
            }
        }

        // cast shadow
        {
            bool castShadow = properties.CastShadow;
            if (ImGuiExt::RenderOptionBool(castShadow, "##renderable_properties_cast_shadow_option", "Cast shadow"))
            {
                hasChanged = true;
                renderable->SetCastShadow(castShadow);
            }
        }

        // use for dynamic env mapping
        {
            bool useForDynamicEnvMapping = properties.UseForDynamicEnvMapping;
            if (ImGuiExt::RenderOptionBool(useForDynamicEnvMapping, "##renderable_properties_dynamic_env_mapping_option", "Dynamic env mapping"))
            {
                hasChanged = true;
                renderable->SetUseForDynamicEnvMapping(useForDynamicEnvMapping);
            }
        }

        // cull distance factor
        {
            float cullDistanceFactor = properties.CullDistanceFactor;
            if (ImGuiExt::RenderOptionFloat(cullDistanceFactor, "##renderable_properties_dynamic_env_mapping_option", "Cull distance factor", 0.0f, 1.0f, width - 20.0f))
            {
                hasChanged = true;
                renderable->SetCullDistanceFactor(cullDistanceFactor);
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowRenderableSubMeshes(SPtr<Renderable> renderable)
    {
        bool hasChanged = false;
        SPtr<Mesh> mesh = renderable->GetMesh();
        const MeshProperties& meshProperties = mesh->GetProperties();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        for (UINT32 i = 0; i < meshProperties.GetNumSubMeshes(); i++)
        {
            const SubMesh& subMesh = meshProperties.GetSubMesh(i);
            String title = "SubMesh " + subMesh.MaterialName;

            if (ImGui::CollapsingHeader(title.c_str()))
            {

            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowScript(SPtr<CScript> script)
    {
        bool hasChanged = false;
        return hasChanged;
    }

    bool WidgetProperties::ShowSkybox(SPtr<Skybox> skybox)
    {
        bool hasChanged = false;
        float brigtness = skybox->GetBrightness();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGuiExt::RenderOptionFloat(brigtness, "##skybox_option", "Brightness", 0.0f, 3.0f, width))
        {
            skybox->SetBrightness(brigtness);
            hasChanged = true;
        }

        return hasChanged;
    }
}
