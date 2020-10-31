#include "TeWidgetMaterials.h"
#include "../TeEditor.h"
#include "../TeEditorResManager.h"
#include "../ImGuiExt/TeImGuiExt.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Gui/TeGuiAPI.h"
#include "Material/TeMaterial.h"
#include "Scene/TeSceneObject.h"
#include "Image/TeTexture.h"
#include "Resources/TeBuiltinResources.h"
#include "Resources/TeResourceManager.h"
#include "Components/TeCRenderable.h"

namespace te
{
    WidgetMaterials::WidgetMaterials()
        : Widget(WidgetType::Game)
        , _currentMaterial(nullptr)
        , _materialCreationCounter(1)
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
        char inputName[128];
        char inputUUID[64];
        UUID empty = UUID(50, 0, 0, 0);
        UUID load  = UUID::EMPTY;
        MaterialProperties properties;
        ImGuiExt::ComboOptions<UUID> materialsOptions;
        UUID materialUUID = (_currentMaterial) ? _currentMaterial->GetUUID() : empty;
        EditorResManager::ResourcesContainer& materials = EditorResManager::Instance().Get<Material>();
        EditorResManager::ResourcesContainer& textures = EditorResManager::Instance().Get<Texture>();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        const auto& ShowTexture = [&](UUID& uuid, bool& textureUsed, const char* id, const char* label, const char* textureName, ImGuiExt::ComboOptions<UUID>& options, float width)
        {
            bool hasChanged = false;
            uuid = empty;

            SPtr<Texture> texture = _currentMaterial->GetTexture(textureName);
            if (texture)
                uuid = texture->GetUUID();

            if (ImGuiExt::RenderOptionCombo<UUID>(&uuid, id, label, options, width))
            {
                if (uuid == load)
                {
                    // TODO
                }
                else if (uuid == empty)
                {
                    _currentMaterial->RemoveTexture(textureName);
                    textureUsed = false;
                    hasChanged = true;
                }
                else
                {
                    _currentMaterial->SetTexture(textureName, gResourceManager().Load<Texture>(uuid).GetInternalPtr());
                    textureUsed = true;
                    hasChanged = true;
                }
            }

            return hasChanged;
        };

        // New material
        {
            ImGui::PushID("##material_new_option");
            if (ImGui::Button(ICON_FA_PLUS_SQUARE " New material", ImVec2(ImGui::GetWindowContentRegionWidth(), 25.0f)))
            {
                HMaterial material = Material::Create(gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque));
                material->SetName("Material " + ToString(_materialCreationCounter));
                material->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
                EditorResManager::Instance().Add(material);
                _currentMaterial = material.GetInternalPtr();
                _materialCreationCounter++;
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
                materialsOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());
            }

            if (ImGuiExt::RenderOptionCombo<UUID>(&materialUUID, "##material_list_option", "", materialsOptions, ImGui::GetWindowContentRegionWidth()))
            {
                if (materialUUID != _currentMaterial->GetUUID())
                    _currentMaterial = gResourceManager().Load<Material>(materialUUID).GetInternalPtr();
            }
        }

        // Current material properties
        if(_currentMaterial)
        {
            properties = _currentMaterial->GetProperties();

            String name = _currentMaterial->GetName();
            String uuid = _currentMaterial->GetUUID().ToString();

            strcpy(inputName, name.c_str());
            strcpy(inputUUID, uuid.c_str());

            if (ImGui::CollapsingHeader("Identification", ImGuiTreeNodeFlags_DefaultOpen))
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

            if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
            {
                {
                    Vector4 color = properties.Ambient.GetAsVector4();
                    if (ImGuiExt::RenderColorRGBA(color, "##material_properties_ambient_option", "Ambient", width))
                    {
                        hasChanged = true;
                        properties.Ambient = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    Vector4 color = properties.Diffuse.GetAsVector4();
                    if (ImGuiExt::RenderColorRGBA(color, "##material_properties_diffuse_option", "Diffuse", width))
                    {
                        hasChanged = true;
                        properties.Diffuse = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    Vector4 color = properties.Specular.GetAsVector4();
                    if (ImGuiExt::RenderColorRGBA(color, "##material_properties_specular_option", "Specular", width))
                    {
                        hasChanged = true;
                        properties.Specular = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionBool(properties.UseDynamicEnvironmentMap, "##material_properties_dynamic_env_mapping_option", "Use dynamic env mapping"))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.SpecularPower, "##material_properties_specular_p_option", "Spec power", 0.0f, 256.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.Transparency, "##material_properties_specular_p_option", "Transparency", 0.0f, 1.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.IndexOfRefraction, "##material_properties_ind_refraction_option", "Ind. refraction", 0.0f, 10.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.Refraction, "##material_properties_refraction_option", "Refraction", 0.0f, 1.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.Reflection, "##material_properties_reflection_option", "Reflection", 0.0f, 1.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.Absorbance, "##material_properties_absorbance_option", "Absorbance", 0.0f, 1.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.BumpScale, "##material_properties_bump_scale_option", "Bump scale", 0.0f, 10.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionFloat(properties.AlphaThreshold, "##material_properties_alpha_threshold_option", "Alpha threshold", 0.0f, 1.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();
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
                        texturesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());

                    if (texture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_CUBE_MAP)
                        texturesEnvMappingOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());
                }

                texturesOptions.AddOption(empty, "No texture");
                texturesOptions.AddOption(load, "Load");

                texturesEnvMappingOptions.AddOption(empty, "No texture");
                texturesEnvMappingOptions.AddOption(load, "Load");

                if (ShowTexture(uuid, properties.UseDiffuseMap, "##material_texture_diffuse_option", "Diffuse", "DiffuseMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseEmissiveMap, "##material_texture_emissive_option", "Emissive", "EmissiveMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseNormalMap, "##material_texture_normal_option", "Normal", "NormalMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseSpecularMap, "##material_texture_specular_option", "Specular", "SpecularMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseParallaxMap, "##material_texture_parallax_option", "Parallax", "ParallaxMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseBumpMap, "##material_texture_bump_option", "Bump", "BumpMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseTransparencyMap, "##material_texture_transparency_option", "Transparency", "TransparencyMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseOcclusionMap, "##material_texture_occlusion_option", "Occlusion", "OcclusionMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseReflectionMap, "##material_texture_reflection_option", "Reflection", "ReflectionMap", texturesOptions, width))
                    hasChanged = true;
                if (ShowTexture(uuid, properties.UseEnvironmentMap, "##material_texture_environment_option", "Environment", "EnvironmentMap", texturesEnvMappingOptions, width))
                    hasChanged = true;
            }

            if (ImGui::CollapsingHeader("Pipeline", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Shader
                {
                    ImGuiExt::ComboOptions<BuiltinShader> shaderTypeOptions;
                    shaderTypeOptions.AddOption(BuiltinShader::Opaque, "Forward opaque");
                    shaderTypeOptions.AddOption(BuiltinShader::Transparent, "Forward transparent");
                    BuiltinShader shaderType = BuiltinShader::Opaque;

                    if (_currentMaterial->GetShader() == gBuiltinResources().GetBuiltinShader(BuiltinShader::Transparent).GetInternalPtr())
                        shaderType = BuiltinShader::Transparent;

                    if (ImGuiExt::RenderOptionCombo<BuiltinShader>(&shaderType, "##material_shader_type_option", "Shader", shaderTypeOptions, width))
                    {
                        _currentMaterial->SetShader(gBuiltinResources().GetBuiltinShader(shaderType).GetInternalPtr());
                        hasChanged = true;
                    }
                }

                // Sampler
                {
                    ImGuiExt::ComboOptions<BuiltinSampler> samplerTypeOptions;
                    samplerTypeOptions.AddOption(BuiltinSampler::Anisotropic, "Anisotropic");
                    samplerTypeOptions.AddOption(BuiltinSampler::Trilinear, "Trilinear");
                    samplerTypeOptions.AddOption(BuiltinSampler::Bilinear, "Bilinear");
                    BuiltinSampler samplerType;

                    if (_currentMaterial->GetSamplerState("AnisotropicSampler") == gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic))
                        samplerType = BuiltinSampler::Anisotropic;
                    else if (_currentMaterial->GetSamplerState("AnisotropicSampler") == gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Trilinear))
                        samplerType = BuiltinSampler::Trilinear;
                    else
                        samplerType = BuiltinSampler::Bilinear;

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
                Vector<HComponent> components = Editor::Instance().GetSceneRoot()->GetComponents(TypeID_Core::TID_CRenderable, true);
                _currentMaterial->SetProperties(properties);

                for (auto& component : components)
                {
                    SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(component.GetInternalPtr());
                    if (renderable->IsUsingMaterial(_currentMaterial))
                        renderable->UpdateMaterials();
                }

                Editor::Instance().NeedsRedraw();
            }
        }
    }

    void WidgetMaterials::UpdateBackground()
    { }
}