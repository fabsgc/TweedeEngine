#include "TeWidgetMaterials.h"

#include "../TeEditor.h"
#include "../TeEditorResManager.h"
#include "../ImGuiExt/TeImGuiExt.h"
#include "../ImGuiExt/TeImGuiFileBrowser.h"
#include "../MaterialsPreview/TeMaterialsPreview.h"
#include "String/TeUnicode.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"
#include "Components/TeCRenderable.h"
#include "Resources/TeResourceManager.h"
#include "Resources/TeBuiltinResources.h"
#include "Importer/TeTextureImportOptions.h"
#include "Image/TePixelUtil.h"

namespace te
{
    WidgetMaterials::WidgetMaterials()
        : Widget(WidgetType::Material)
        , _currentMaterial(nullptr)
        , _materialCreationCounter(1)
        , _loadTexture(false)
        , _loadTextureUsed(nullptr)
        , _fileBrowser(gEditor().GetFileBrowser())
        , _materialsPreview(gEditor().GetMaterialsPreview())
    {
        _title = MATERIALS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetMaterials::~WidgetMaterials()
    { }

    void WidgetMaterials::Initialize()
    { }

    void WidgetMaterials::Update()
    { 
        bool hasChanged = false;
        char inputName[256];
        char inputUUID[64];
        UUID load = UUID::EMPTY;
        UUID empty = UUID(50, 0, 0, 0);
        MaterialProperties properties;
        float offsetListMaterials = 0.0f;
        SPtr<Texture> currentTexture = nullptr;
        ImGuiExt::ComboOptions<UUID> materialsOptions;
        UINT8 flags = (UINT8)ImGuiExt::ComboOptionFlag::ShowTexture;
        UUID materialUUID = (_currentMaterial) ? _currentMaterial->GetUUID() : empty;
        EditorResManager::ResourcesContainer& materials = EditorResManager::Instance().Get<Material>();
        EditorResManager::ResourcesContainer& textures = EditorResManager::Instance().Get<Texture>();
        const float width = ImGui::GetWindowContentRegionWidth() - 110.0f;

        const auto& ShowTexture = [&](UUID& uuid, bool& textureUsed, const char* id, const char* label, const char* textureName, 
            ImGuiExt::ComboOptions<UUID>& options, float width, bool disable, TextureType texType, bool SRGB, bool isNormalMap)
        {
            SPtr<Texture> texture = nullptr;
            bool hasChanged = false;
            uuid = empty;
            UINT8 flags = (UINT8)ImGuiExt::ComboOptionFlag::ShowTexture;

            if (textureUsed)
            {
                texture = _currentMaterial->GetTexture(textureName);
                if (texture) uuid = texture->GetUUID();
            }

            if (disable)
                flags |= (UINT8)ImGuiExt::ComboOptionFlag::Disable;

            if (texture && texture->GetProperties().GetTextureType() != TextureType::TEX_TYPE_CUBE_MAP)
            {
                ImGuiExt::RenderImage(texture, Vector2(26.0f, 26.0f));
                ImGui::SameLine();

                ImVec2 cursor = ImGui::GetCursorPos();
                cursor.x -= 5.0f;
                ImGui::SetCursorPos(cursor);

                width -= 26.0f;
            }

            if (ImGuiExt::RenderOptionCombo<UUID>(&uuid, id, label, options, width, flags))
            {
                if (uuid == load)
                {
                    _loadTexture = true;
                    _loadTextureName = textureName;
                    _loadTextureUsed = &textureUsed;
                    _fileBrowser.Data.TexParam.TexType = texType;
                    _fileBrowser.Data.TexParam.SRGB = SRGB;
                    _fileBrowser.Data.TexParam.IsNormalMap = isNormalMap;

                    if(texType == TextureType::TEX_TYPE_CUBE_MAP)
                    {
                        _fileBrowser.Data.TexParam.GenerateMips = false;
                    }
                    else
                    {
                        _fileBrowser.Data.TexParam.GenerateMips = true;
                        _fileBrowser.Data.TexParam.GenerateMipsOnGpu = true;
                        _fileBrowser.Data.TexParam.MipsPreserveCoverage = true;
                        _fileBrowser.Data.TexParam.MaxMips = 0;
                    }
                }
                else if (uuid == empty)
                {
                    _currentMaterial->RemoveTexture(textureName);
                    textureUsed = false;
                    hasChanged = true;
                }
                else
                {
                    HTexture loadedTexture = gResourceManager().Load<Texture>(uuid);
                    if (loadedTexture.IsLoaded() && loadedTexture->GetProperties().GetTextureType() == texType)
                    {
                        _currentMaterial->SetTexture(textureName, loadedTexture.GetInternalPtr());
                        textureUsed = true;
                        hasChanged = true;
                    }
                }
            }

            return hasChanged;
        };

        const auto& ShowPreviewButton = [this](const char* title, const std::function<bool()>& getVisibility)
        {
            float buttonWidth = ImGui::GetWindowContentRegionWidth() / 3.0f - 4.0f;

            bool clicked = false;
            ImGui::PushStyleColor(ImGuiCol_Button, getVisibility() ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);

            if (ImGui::Button(title, ImVec2(buttonWidth, 24.0f)))
                clicked = true;

            ImGui::PopStyleColor();

            return clicked;
        };

        // New material
        {
            ImGui::PushID("##material_new_option");
            if (ImGui::Button(ICON_FA_PLUS_SQUARE " New material", ImVec2(ImGui::GetWindowContentRegionWidth(), 25.0f)))
            {
                HMaterial material = Material::Create(gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque));
                material->SetName("Material " + ToString(_materialCreationCounter));
                EditorResManager::Instance().Add(material);
                _currentMaterial = material.GetInternalPtr();
                _materialCreationCounter++;
                materialUUID = _currentMaterial->GetUUID();
            }
            ImGui::PopID();
        }

        // Materials list
        {
            for (auto& resource : materials.Res)
            {
                if (!_currentMaterial)
                {
                    _currentMaterial = gResourceManager().Load<Material>(resource.second->GetUUID()).GetInternalPtr();
                    materialUUID = _currentMaterial->GetUUID();
                }

                SPtr<Texture> texture = _materialsPreview.GetPreview(
                    static_resource_cast<Material>(resource.second).GetInternalPtr())
                    .RenderTex->GetColorTexture(0);

                materialsOptions.AddOption(resource.second->GetUUID(), resource.second->GetName(), texture);
            }

            if (_currentMaterial)
            {
                currentTexture = _materialsPreview.GetPreview(_currentMaterial).RenderTex->GetColorTexture(0);
                if (currentTexture && currentTexture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_2D)
                {
                    ImGuiExt::RenderImage(currentTexture, Vector2(26.0f, 26.0f));
                    ImGui::SameLine();

                    ImVec2 cursor = ImGui::GetCursorPos();
                    cursor.x -= 5.0f;
                    ImGui::SetCursorPos(cursor);

                    offsetListMaterials = 26.0f;
                }

                if (ImGuiExt::RenderOptionCombo<UUID>(&materialUUID, "##material_list_option", "", 
                    materialsOptions, ImGui::GetWindowContentRegionWidth() - 32.0f - offsetListMaterials, flags))
                {
                    if (materialUUID != _currentMaterial->GetUUID())
                        _currentMaterial = gResourceManager().Load<Material>(materialUUID).GetInternalPtr();
                }

                // You can delete a material
                // Be careful this feature also reset all renderables which are using this material
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_TIMES_CIRCLE, ImVec2(25.0f, 26.0f)))
                {
                    DeleteMaterial(_currentMaterial, materialUUID);
                }
            }
        }

        // Current material properties
        if(_currentMaterial)
        {
            properties = _currentMaterial->GetProperties();

            String name = _currentMaterial->GetName();
            String uuidStr = _currentMaterial->GetUUID().ToString();

            memset(&inputName, 0, 256);
            strcpy(inputUUID, uuidStr.c_str());

            if(name.length() < 256) strcpy(inputName, name.c_str());
            else strcpy(inputName, name.substr(0,255).c_str());

            if (ImGui::CollapsingHeader("Identification"))
            {
                ImGui::PushItemWidth(width);
                if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
                    _currentMaterial->SetName(inputName);
                ImGui::Separator();

                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
                ImGui::PopItemFlag();
                ImGui::PopItemWidth();
            }

            if (ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen))
            {
                float previewZoneWidth = ImGui::GetWindowContentRegionWidth();
                float previewWidth = (previewZoneWidth > 256.0f) ? 256.0f : previewZoneWidth;
                float previewHeight = previewWidth;
                float previewOffset = 0.0f;

                if (previewWidth < 16.0f)
                    previewWidth = 16.0f;

                if (previewZoneWidth > 256.0f)
                    previewOffset = (previewZoneWidth - previewWidth) * 0.5f;

                ImGui::BeginChild("TexturePreview", ImVec2(previewZoneWidth, previewHeight), true, ImGuiWindowFlags_NoScrollbar);
                ImGuiExt::RenderImage(currentTexture, Vector2(previewWidth - 16.0f, previewHeight - 16.0f), Vector2(previewOffset, 0.0f));
                ImGui::EndChild();

                ImVec2 cursor = ImGui::GetCursorPos();
                cursor.y -= 7.0f;
                ImGui::SetCursorPos(cursor);

                if (ShowPreviewButton(ICON_FA_CIRCLE, [this]() { 
                    return _materialsPreview.GetMeshPreviewType() != MaterialsPreview::MeshPreviewType::Sphere; }))
                {
                    _materialsPreview.SetMeshPreviewType(MaterialsPreview::MeshPreviewType::Sphere);
                }
                ImGui::SameLine();

                if (ShowPreviewButton(ICON_FA_CUBE, [this]() {
                    return _materialsPreview.GetMeshPreviewType() != MaterialsPreview::MeshPreviewType::Box; }))
                {
                    _materialsPreview.SetMeshPreviewType(MaterialsPreview::MeshPreviewType::Box);
                }
                ImGui::SameLine();

                if (ShowPreviewButton(ICON_FA_SQUARE, [this]() {
                    return _materialsPreview.GetMeshPreviewType() != MaterialsPreview::MeshPreviewType::Plane; }))
                {
                    _materialsPreview.SetMeshPreviewType(MaterialsPreview::MeshPreviewType::Plane);
                }

                cursor.x -= 5.0f;
                ImGui::SetCursorPos(cursor);

                cursor.y += 40.0f;
                ImGui::SetCursorPos(cursor);
            }

            if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
            {
                {
                    Vector4 color = properties.BaseColor.GetAsVector4();
                    if (ImGuiExt::RenderColorRGB(color, "##material_prop_base_color_option", "Base Color", width, properties.UseBaseColorMap))
                    {
                        hasChanged = true;
                        properties.BaseColor = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    Vector4 color = properties.Emissive.GetAsVector4();
                    if (ImGuiExt::RenderColorRGB(color, "##material_prop_emissive_option", "Emissive Color", width))
                    {
                        hasChanged = true;
                        properties.Emissive = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    Vector4 color = properties.SheenColor.GetAsVector4();
                    if (ImGuiExt::RenderColorRGB(color, "##material_prop_sheen_color_option", "Sheen Color", width, properties.UseSheenColorMap))
                    {
                        hasChanged = true;
                        properties.SheenColor = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    Vector4 color = properties.SubsurfaceColor.GetAsVector4();
                    if (ImGuiExt::RenderColorRGB(color, "##material_prop_subsurface_color_option", "SubSurface Color", width))
                    {
                        hasChanged = true;
                        properties.SubsurfaceColor = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    Vector4 color = properties.TransmittanceColor.GetAsVector4();
                    if (ImGuiExt::RenderColorRGB(color, "##material_prop_transmittance_color_option", "Transmittance Color", width))
                    {
                        hasChanged = true;
                        properties.TransmittanceColor = Color(color);
                        properties.ComputeAbsorption();
                    }
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionBool(properties.DoIndirectLighting, "##material_properties_do_indirect_lighting_option", "Use Indirect Lighting"))
                        hasChanged = true;
                    if (ImGuiExt::RenderOptionBool(properties.DoDirectLighting, "##material_properties_do_direct_lighting_option", "Use Direct Lighting"))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Metallic, "##material_prop_metallic_option", "Metallic", 0.0f, 1.0f, width, properties.UseMetallicMap))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Roughness, "##material_prop_roughness_option", "Roughness", 0.0f, 1.0f, width, properties.UseRoughnessMap))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Reflectance, "##material_prop_reflectance_option", "Reflectance", 0.0f, 1.0f, width, properties.UseReflectanceMap))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Occlusion, "##material_prop_occlusion_option", "Occlusion", 0.0f, 1.0f, width, properties.UseOcclusionMap))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.SheenRoughness, "##material_prop_sheen_roughness_option", "Sheen Rough.", 0.0f, 1.0f, width, properties.UseSheenRoughnessMap))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.ClearCoat, "##material_prop_clear_coat_option", "Clear Coat", 0.0f, 1.0f, width, properties.UseClearCoatMap))
                        hasChanged = true;
                    if (ImGuiExt::RenderOptionFloat(properties.ClearCoatRoughness, "##material_prop_clear_coat_roughness_option", "Clear C. Rough.", 0.0f, 1.0f, width, properties.UseClearCoatRoughnessMap))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Anisotropy, "##material_prop_anisotropy_option", "Anisotropy", -1.0f, 1.0f, width))
                        hasChanged = true;
                    if (ImGuiExt::RenderVector3(properties.AnisotropyDirection, "##material_prop_anisotropy_direction_option", " Anisotropy Dir.", width))
                    {
                        properties.AnisotropyDirection.Normalize();
                        hasChanged = true;
                    }
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.ParallaxScale, "##material_prop_parallax_scale_option", "Parallax Scale", -1.0f, 1.0f, width))
                        hasChanged = true;
                    if (ImGuiExt::RenderOptionInt((int&)properties.ParallaxSamples, "##material_prop_parallax_samples_option", "Parallax Samples", 16, 256, width))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Thickness, "##material_prop_thickness_option", "Thickness", 0.0f, 2.0f, width))
                        hasChanged = true;
                    if (ImGuiExt::RenderOptionFloat(properties.MicroThickness, "##material_prop_micto_thickness_option", "Micro Thickness", 0.0f, 2.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.SubsurfacePower, "##material_prop_subsurface_power_option", "Sub. power", 0.0f, 64.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Transmission, "##material_prop_transmission_option", "Transmission", 0.0f, 1.0f, width, properties.UseTransmissionMap))
                        hasChanged = true;
                    if (ImGuiExt::RenderOptionFloat(properties.AlphaThreshold, "##material_prop_alpha_threshold_option", "Alpha Threshold", 0.0f, 1.0f, width))
                        hasChanged = true;
                }

                {
                    static ImGuiExt::ComboOptions<RefractionType> refractionTypeOptions;
                    if (refractionTypeOptions.Options.size() == 0)
                    {
                        refractionTypeOptions.AddOption(RefractionType::Solid, "Solid");
                        refractionTypeOptions.AddOption(RefractionType::Thin, "Thin");
                    }

                    RefractionType* refractionType = &properties.RefractType;

                    if (ImGuiExt::RenderOptionCombo<RefractionType>(refractionType, "##material_prop_refract_type_option", "Refract. type", refractionTypeOptions, width))
                    {
                        hasChanged = true;
                        properties.ComputeAbsorption();
                    }
                }

                {    
                    if (ImGuiExt::RenderOptionFloat(properties.AtDistance, "##material_prop_at_distance_option", "At Distance", 0.0f, 16.0f, width))
                    {
                        hasChanged = true;
                        properties.ComputeAbsorption();
                    }
                }

                {
                    if (ImGuiExt::RenderVector3(properties.Absorption, "##material_prop_absorption_option", " Absorp.", width))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderVector2(properties.TextureRepeat, "##material_properties_texture_repeat_option", " UV Repeat", width))
                        hasChanged = true;
                    if (ImGuiExt::RenderVector2(properties.TextureOffset, "##material_properties_texture_repeat_option", " UV Offset", width))
                        hasChanged = true;
                }
            }

            if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
            {
                UUID uuid;
                ImGuiExt::ComboOptions<UUID> texturesOptions;
                ImGuiExt::ComboOptions<UUID> texturesEnvMappingOptions;
                
                for (auto& resource : textures.Res)
                {
                    SPtr<Texture> texture = std::static_pointer_cast<Texture>(resource.second.GetInternalPtr());
                    if (texture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_2D)
                    {
                        texturesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName(),
                            std::static_pointer_cast<Texture>(resource.second.GetInternalPtr()));
                    }

                    if (texture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_CUBE_MAP)
                    {
                        texturesEnvMappingOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());
                    }
                }

                texturesOptions.AddOption(empty, ICON_FA_TIMES_CIRCLE " No texture");
                texturesOptions.AddOption(load, ICON_FA_FOLDER_OPEN " Load");

                texturesEnvMappingOptions.AddOption(empty, ICON_FA_TIMES_CIRCLE " No texture");
                texturesEnvMappingOptions.AddOption(load, ICON_FA_FOLDER_OPEN " Load");

                if (ShowTexture(uuid, properties.UseBaseColorMap, "##material_texture_base_color_option", "Base Color", "BaseColorMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, true, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseMetallicMap, "##material_texture_metallic_option", "Metallic", "MetallicMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseRoughnessMap, "##material_texture_roughness_option", "Roughness", "RoughnessMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseMetallicRoughnessMap, "##material_texture_metallic_roughness_option", "Metallic/Rough.", "MetallicRoughnessMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseReflectanceMap, "##material_texture_reflectance_option", "Reflectance", "ReflectanceMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseOcclusionMap, "##material_texture_occlusion_option", "Occlusion", "OcclusionMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseEmissiveMap, "##material_texture_emissive_option", "Emissive", "EmissiveMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, true, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseSheenColorMap, "##material_texture_sheen_color_option", "Sheen Color", "SheenColorMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, true, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseSheenRoughnessMap, "##material_texture_sheen_roughness_option", "Sheen Rough.", "SheenRoughnessMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseClearCoatMap, "##material_texture_clear_coat_option", "Clear Coat", "ClearCoatMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseClearCoatRoughnessMap, "##material_texture_clear_coat_roughness_option", "Clear C. Rough.", "ClearCoatRoughnesssMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseClearCoatNormalMap, "##material_texture_clear_coat_normal_option", "Clear C. Normal", "ClearCoatNormalMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, true))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseNormalMap, "##material_texture_normal_option", "Normal", "NormalMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, true))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseParallaxMap, "##material_texture_parallax_option", "Parallax", "ParallaxMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseTransmissionMap, "##material_texture_transmission_option", "Transmission", "TransmissionMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseOpacityMap, "##material_texture_opacity_option", "Opacity", "OpacityMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, false))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseAnisotropyDirectionMap, "##material_texture_anisotropy_direction_option", "Anisotropy Dir.", "AnisotropyDirectionMap", 
                    texturesOptions, width, false, TextureType::TEX_TYPE_2D, false, true))
                    hasChanged = true;

                if (ShowLoadedTexture())
                    hasChanged = true;
            }

            if (ImGui::CollapsingHeader("Pipeline", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Shader
                {
                    static ImGuiExt::ComboOptions<BuiltinShader> shaderTypeOptions;
                    BuiltinShader shaderType = BuiltinShader::Opaque;

                    if (shaderTypeOptions.Options.size() == 0)
                    {
                        shaderTypeOptions.AddOption(BuiltinShader::Opaque, "Forward opaque");
                        shaderTypeOptions.AddOption(BuiltinShader::Transparent, "Forward transparent");
                        shaderTypeOptions.AddOption(BuiltinShader::TransparentCullNone, "Forward transparent no Cull");
                    }

                    if (_currentMaterial->GetShader() == gBuiltinResources().GetBuiltinShader(BuiltinShader::Transparent).GetInternalPtr())
                        shaderType = BuiltinShader::Transparent;

                    if(_currentMaterial->GetShader() == gBuiltinResources().GetBuiltinShader(BuiltinShader::TransparentCullNone).GetInternalPtr())
                        shaderType = BuiltinShader::TransparentCullNone;

                    if (ImGuiExt::RenderOptionCombo<BuiltinShader>(&shaderType, "##material_shader_type_option", "Shader", shaderTypeOptions, width))
                    {
                        _currentMaterial->SetShader(gBuiltinResources().GetBuiltinShader(shaderType).GetInternalPtr());
                        hasChanged = true;
                    }
                }

                // Sampler
                {
                    static ImGuiExt::ComboOptions<BuiltinSampler> samplerTypeOptions;
                    BuiltinSampler samplerType;

                    if (samplerTypeOptions.Options.size() == 0)
                    {
                        samplerTypeOptions.AddOption(BuiltinSampler::Anisotropic, "Anisotropic");
                        samplerTypeOptions.AddOption(BuiltinSampler::Trilinear, "Trilinear");
                        samplerTypeOptions.AddOption(BuiltinSampler::Bilinear, "Bilinear");
                        samplerTypeOptions.AddOption(BuiltinSampler::NoFilter, "No Filter");
                    }

                    if (_currentMaterial->GetSamplerState("AnisotropicSampler") == gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic))
                        samplerType = BuiltinSampler::Anisotropic;
                    else if (_currentMaterial->GetSamplerState("AnisotropicSampler") == gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Trilinear))
                        samplerType = BuiltinSampler::Trilinear;
                    else if (_currentMaterial->GetSamplerState("AnisotropicSampler") == gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear))
                        samplerType = BuiltinSampler::Bilinear;
                    else
                        samplerType = BuiltinSampler::NoFilter;

                    if (ImGuiExt::RenderOptionCombo<BuiltinSampler>(&samplerType, "##material_sampler_type_option", "Sampler", samplerTypeOptions, width))
                    {
                        _currentMaterial->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(samplerType));
                        hasChanged = true;
                    }
                }
            }

            if (hasChanged)
            {
                // We must update gpu params on all renderables who are using this material
                Vector<HComponent> components = gEditor().GetSceneRoot()->GetComponents(TypeID_Core::TID_CRenderable, true);
                _currentMaterial->SetProperties(properties);

                for (auto& component : components)
                {
                    SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(component.GetInternalPtr());
                    if (renderable->IsUsingMaterial(_currentMaterial))
                        renderable->UpdateMaterials();
                }

                gEditor().NeedsRedraw();
                gEditor().GetSettings().State = Editor::EditorState::Modified;

                _materialsPreview.MarkDirty(_currentMaterial);
            }
        }
    }

    void WidgetMaterials::UpdateBackground()
    { }

    bool WidgetMaterials::ShowLoadedTexture()
    {
        bool textureLoaded = false;

        if (_loadTexture)
            ImGui::OpenPopup("Load Material Texture");

        if (_fileBrowser.ShowFileDialog("Load Material Texture", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, Editor::TexturesExtensionsStr))
        {
            auto textureImportOptions = TextureImportOptions::Create();
            textureImportOptions->CpuCached = _fileBrowser.Data.TexParam.CpuCached;
            textureImportOptions->GenerateMips = _fileBrowser.Data.TexParam.GenerateMips;
            textureImportOptions->GenerateMipsOnGpu = _fileBrowser.Data.TexParam.GenerateMipsOnGpu;
            textureImportOptions->MaxMip = _fileBrowser.Data.TexParam.MaxMips;
            textureImportOptions->Format = PixelUtil::BestFormatFromFile(_fileBrowser.Data.SelectedPath);
            textureImportOptions->SRGB = _fileBrowser.Data.TexParam.SRGB;
            textureImportOptions->IsNormalMap = _fileBrowser.Data.TexParam.IsNormalMap;

            if (_fileBrowser.Data.TexParam.TexType == TextureType::TEX_TYPE_CUBE_MAP)
            {
                textureImportOptions->CubemapType = CubemapSourceType::Faces;
                textureImportOptions->IsCubeMap = true;
                textureImportOptions->IsNormalMap = false;
            }

            HTexture texture = EditorResManager::Instance().Load<Texture>(_fileBrowser.Data.SelectedPath, textureImportOptions);
            if (texture.IsLoaded())
            {
                // Load texture from file
                texture->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));

                // Set texture and reset _loadTextureName and _loadTextureUsed
                _currentMaterial->SetTexture(_loadTextureName, texture);
                _loadTextureName.clear();
                *_loadTextureUsed = true;

                textureLoaded = true;
            }

            _loadTexture = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _loadTexture = false;
        }

        return textureLoaded;
    }

    void WidgetMaterials::DeleteMaterial(SPtr<Material> material, const UUID& uuid)
    {
        HMaterial handle = static_resource_cast<Material>(
            gResourceManager()._createResourceHandle(material, uuid));

        Vector<HComponent> renderables = gSceneManager().GetRootNode()->GetComponents<CRenderable>(true);

        for (auto& component : renderables)
        {
            HRenderable renderable = static_object_cast<CRenderable>(component);
            renderable->RemoveMaterial(material);
        }

        _materialsPreview.DeletePreview(material);

        EditorResManager::Instance().Remove<Material>(handle);
        material = nullptr;
        _currentMaterial = nullptr;
        gEditor().NeedsRedraw();
    }
}
