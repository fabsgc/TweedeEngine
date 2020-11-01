#include "TeWidgetProperties.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "../TeEditorResManager.h"
#include "Resources/TeResourceManager.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCCamera.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCScript.h"
#include "Components/TeCLight.h"
#include "RenderAPI/TeSubMesh.h"
#include "Mesh/TeMesh.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"

namespace te
{
    WidgetProperties::WidgetProperties()
        : Widget(WidgetType::Properties)
        , _selections(gEditor().GetSelectionData())
        , _fileBrowser(gEditor().GetFileBrowser())
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
        {
            gEditor().NeedsRedraw();
            gEditor().GetSettings().State = Editor::EditorState::Modified;
        }
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

        if (renderable->GetMesh())
        {
            if (ImGui::CollapsingHeader("SubMeshes", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ShowRenderableSubMeshes(renderable->_getInternal()))
                    hasChanged = true;
            }
        }
        
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
            static ImGuiExt::ComboOptions<int> mobilityOptions;

            if (mobilityOptions.Options.size() == 0)
            {
                mobilityOptions.AddOption((int)ObjectMobility::Movable, "Movable");
                mobilityOptions.AddOption((int)ObjectMobility::Immovable, "Immovable");
                mobilityOptions.AddOption((int)ObjectMobility::Static, "Static");
            }

            if (ImGuiExt::RenderOptionCombo<int>((int*)(&mobility), "##transform_mobility_option", "Mobility", mobilityOptions, widgetWidth, disableTransform))
                hasChanged = true;

            ImGui::Separator();

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
            ImGui::Separator();

            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
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
            camera->SetRenderSettings(cameraSettings);

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
        bool castsShadows = light->GetCastsShadow();
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
        ImGui::Separator();

        // Cast shadows
        {
            if (ImGuiExt::RenderOptionBool(castsShadows, "##light_cast_shadows_option", "Cast shadows"))
            {
                hasChanged = true;
                light->SetCastsShadow(castsShadows);
            }
        }
        ImGui::Separator();

        // Light Type
        {
            static ImGuiExt::ComboOptions<int> lightTypeOptions;
            if (lightTypeOptions.Options.size() == 0)
            {
                lightTypeOptions.AddOption((int)LightType::Directional, "Directional");
                lightTypeOptions.AddOption((int)LightType::Radial, "Radial");
                lightTypeOptions.AddOption((int)LightType::Spot, "Spot");
            }

            if (ImGuiExt::RenderOptionCombo<int>((int*)(&lightType), "##light_type_option", "Type", lightTypeOptions, width, true))
            {
                light->SetType((LightType)lightType);
                hasChanged = true;
            }
        }
        ImGui::Separator();

        // Attenuation radius
        {
            if (ImGuiExt::RenderOptionFloat(attenuationRadius, "##light_attenuation_radius_option", "Att. Radius", 0.0f, 5.0f, width))
            {
                hasChanged = true;
                light->SetAttenuationRadius(attenuationRadius);
            }
        }
        ImGui::Separator();

        // Linear attenuation
        {
            if (ImGuiExt::RenderOptionFloat(linearAttenuation, "##light_linear_attenuation_option", "Linear att.", 0.0f, 1.0f, width))
            {
                hasChanged = true;
                light->SetLinearAttenuation(linearAttenuation);
            }
        }
        ImGui::Separator();

        // quadratic attenuation
        {
            if (ImGuiExt::RenderOptionFloat(quadraticAttenuation, "##light_quadratic_attenuation_option", "Quadratic att.", 0.0f, 1.0f, width))
            {
                hasChanged = true;
                light->SetQuadraticAttenuation(quadraticAttenuation);
            }
        }
        ImGui::Separator();

        // intensity
        {
            if (ImGuiExt::RenderOptionFloat(intensity, "##light_intensity_option", "Intensity", 0.0f, 5.0f, width))
            {
                hasChanged = true;
                light->SetIntensity(intensity);
            }
        }
        ImGui::Separator();

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
            ImGui::Separator();
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
        const SPtr<Mesh> mesh = renderable->GetMesh();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        ImGuiExt::ComboOptions<UUID> meshesOptions;
        UUID emptyMesh = UUID(50, 0, 0, 0);
        UUID loadMesh = UUID::EMPTY;
        UUID meshUUID = (mesh) ? mesh->GetUUID() : emptyMesh;
        EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<Mesh>();

        // current mesh to use
        {
            for (auto& resource : container.Res)
                meshesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());

            meshesOptions.AddOption(emptyMesh, "No mesh");
            meshesOptions.AddOption(UUID::EMPTY, ICON_FA_FOLDER_OPEN " Load");

            if (ImGuiExt::RenderOptionCombo<UUID>(&meshUUID, "##meshes_option", "Mesh", meshesOptions, width))
            {
                if (meshUUID == loadMesh)
                {
                    _loadMesh = true;
                }
                else if (meshUUID == emptyMesh)
                {
                    renderable->SetMesh(nullptr);
                    renderable->ClearAllMaterials();
                    hasChanged = true;
                }
                else if (meshUUID != ((mesh) ? mesh->GetUUID() : emptyMesh))
                {
                    renderable->SetMesh(gResourceManager().Load<Mesh>(meshUUID).GetInternalPtr());
                    renderable->ClearAllMaterials();
                    hasChanged = true;
                }
            }

            if (ShowLoadMesh())
                hasChanged = true;
        }
        ImGui::Separator();

        // instancing
        {
            bool instancing = properties.Instancing;
            if (ImGuiExt::RenderOptionBool(instancing, "##renderable_properties_instancing_option", "Instancing"))
            {
                hasChanged = true;
                renderable->SetInstancing(instancing);
            }
        }
        ImGui::Separator();

        // can be merged
        {
            bool canBeMerged = properties.CanBeMerged;
            if (ImGuiExt::RenderOptionBool(canBeMerged, "##renderable_properties_can_be_merged_option", "Merging"))
            {
                hasChanged = true;
                renderable->SetCanBeMerged(canBeMerged);
            }
        }
        ImGui::Separator();

        // cast shadows
        {
            bool castShadow = properties.CastShadow;
            if (ImGuiExt::RenderOptionBool(castShadow, "##renderable_properties_cast_shadow_option", "Cast shadows"))
            {
                hasChanged = true;
                renderable->SetCastShadow(castShadow);
            }
        }
        ImGui::Separator();

        // use for dynamic env mapping
        {
            bool useForDynamicEnvMapping = properties.UseForDynamicEnvMapping;
            if (ImGuiExt::RenderOptionBool(useForDynamicEnvMapping, "##renderable_properties_dynamic_env_mapping_option", "Dynamic env mapping"))
            {
                hasChanged = true;
                renderable->SetUseForDynamicEnvMapping(useForDynamicEnvMapping);
            }
        }
        ImGui::Separator();

        // cull distance factor
        {
            float cullDistanceFactor = properties.CullDistanceFactor;
            if (ImGuiExt::RenderOptionFloat(cullDistanceFactor, "##renderable_properties_dynamic_env_mapping_option", "Cull dist. factor", 0.0f, 1.0f, width - 20.0f))
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
        ImGuiExt::ComboOptions<UUID> materialsOptions;
        UUID emptyMaterial = UUID(50, 0, 0, 0);
        const MeshProperties& meshProperties = mesh->GetProperties();
        const float width = ImGui::GetWindowContentRegionWidth() - 120.0f;
        EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<Material>();

        for (auto& resource : container.Res)
            materialsOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());

        materialsOptions.AddOption(emptyMaterial, "No material");

        for (UINT32 i = 0; i < meshProperties.GetNumSubMeshes(); i++)
        {
            SPtr<Material> material = renderable->GetMaterial(i);
            UUID materialUUID = (material) ? material->GetUUID() : emptyMaterial;
            const SubMesh& subMesh = meshProperties.GetSubMesh(i);
            String title = subMesh.MaterialName;

            // current material to use
            {
                if (ImGuiExt::RenderOptionCombo<UUID>(&materialUUID, "##submesh_material_option", title.c_str(), materialsOptions, width))
                {
                    if (materialUUID == emptyMaterial)
                    {
                        renderable->SetMaterial(i, nullptr);
                        hasChanged = true;
                    }
                    else
                    {
                        renderable->SetMaterial(i, gResourceManager().Load<Material>(materialUUID).GetInternalPtr());
                        hasChanged = true;
                    }
                }
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

        ImGuiExt::ComboOptions<UUID> texturesOptions;
        UUID emptyTexture = UUID(50, 0, 0, 0);
        UUID loadTexture = UUID::EMPTY;
        UUID textureUUID = (skybox->GetTexture()) ? skybox->GetTexture()->GetUUID() : emptyTexture;
        EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<Texture>();

        // current texture to use
        {
            for (auto& resource : container.Res)
            {
                SPtr<Texture> texture = std::static_pointer_cast<Texture>(resource.second.GetInternalPtr());
                if(texture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_CUBE_MAP)
                    texturesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());
            }

            texturesOptions.AddOption(emptyTexture, "No texture");
            texturesOptions.AddOption(UUID::EMPTY, ICON_FA_FOLDER_OPEN " Load");

            if (ImGuiExt::RenderOptionCombo<UUID>(&textureUUID, "##skybox_texture_option", "Texture", texturesOptions, width))
            {
                if (textureUUID == loadTexture)
                {
                    _loadSkybox = true;
                }
                else if (textureUUID == emptyTexture)
                {
                    skybox->SetTexture(nullptr);
                    hasChanged = true;
                }
                else
                {
                    skybox->SetTexture(gResourceManager().Load<Texture>(textureUUID).GetInternalPtr());
                    hasChanged = true;
                }
            }
        }
        ImGui::Separator();

        if (ImGuiExt::RenderOptionFloat(brigtness, "##skybox_option", "Brightness", 0.0f, 3.0f, width))
        {
            skybox->SetBrightness(brigtness);
            hasChanged = true;
        }

        if (ShowLoadSkybox())
            hasChanged = true;

        return hasChanged;
    }

    bool WidgetProperties::ShowLoadMesh()
    {
        bool meshLoaded = false;

        if (_loadMesh)
            ImGui::OpenPopup("Load Mesh");

        if (_fileBrowser.ShowFileDialog("Load Mesh", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(800, 450), true, ".obj,.dae,.fbx"))
        {
            auto meshImportOptions = MeshImportOptions::Create();
            meshImportOptions->ImportNormals = _fileBrowser.Data.MeshParam.ImportNormals;
            meshImportOptions->ImportTangents = _fileBrowser.Data.MeshParam.ImportTangents;
            meshImportOptions->ImportSkin = _fileBrowser.Data.MeshParam.ImportSkin;
            meshImportOptions->ImportAnimation = _fileBrowser.Data.MeshParam.ImportAnimation;
            meshImportOptions->CpuCached = false;

            HMesh mesh = EditorResManager::Instance().Load<Mesh>(_fileBrowser.Data.SelectedPath, meshImportOptions);
            if (mesh.GetHandleData())
            {
                mesh->SetName(_fileBrowser.Data.SelectedFileName);
                EditorResManager::Instance().Add<Mesh>(mesh);
                SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(_selections.ClickedComponent);
                renderable->SetMesh(mesh.GetInternalPtr());
                meshLoaded = true;
            }

            _loadMesh = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _loadMesh = false;
        }

        return meshLoaded;
    }

    bool WidgetProperties::ShowLoadSkybox()
    {
        bool textureLoaded = false;

        if (_loadSkybox)
            ImGui::OpenPopup("Load Skybox Texture");

        if (_fileBrowser.ShowFileDialog("Load Skybox Texture", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(800, 450), false, ".jpeg,.jpg,.png"))
        {
            auto textureSkyboxImportOptions = TextureImportOptions::Create();
            textureSkyboxImportOptions->CpuCached = false;
            textureSkyboxImportOptions->CubemapType = CubemapSourceType::Faces;
            textureSkyboxImportOptions->Format = PF_RGBA8;
            textureSkyboxImportOptions->IsCubemap = true;

            HTexture texture = EditorResManager::Instance().Load<Texture>(_fileBrowser.Data.SelectedPath, textureSkyboxImportOptions);
            if (texture.GetHandleData())
            {
                texture->SetName(_fileBrowser.Data.SelectedFileName);
                EditorResManager::Instance().Add<Texture>(texture);
                SPtr<CSkybox> skybox = std::static_pointer_cast<CSkybox>(_selections.ClickedComponent);
                skybox->SetTexture(texture.GetInternalPtr());
                textureLoaded = true;
            }

            _loadSkybox = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _loadSkybox = false;
        }

        return textureLoaded;
    }
}
