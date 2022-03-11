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

namespace te
{
    WidgetMaterials::WidgetMaterials()
        : Widget(WidgetType::Material)
        , _currentMaterial(nullptr)
        , _materialCreationCounter(1)
        , _loadTexture(false)
        , _loadTextureUsed(nullptr)
        , _fileBrowser(gEditor().GetFileBrowser())
    {
        _title = MATERIALS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetMaterials::~WidgetMaterials()
    { }

    void WidgetMaterials::Initialize()
    { 
        _materialsPreview = te_unique_ptr_new<MaterialsPreview>();
    }

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
            ImGuiExt::ComboOptions<UUID>& options, float width, bool disable = false, TextureType texType = TextureType::TEX_TYPE_2D)
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
                ImGuiExt::RenderImage(texture, 3, Vector2(26.0f, 26.0f));
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
                material->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
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

                SPtr<Texture> texture = _materialsPreview->GetPreview(
                    static_resource_cast<Material>(resource.second).GetInternalPtr())
                    .RenderTex->GetColorTexture(0);

                materialsOptions.AddOption(resource.second->GetUUID(), resource.second->GetName(), texture);
            }

            if (_currentMaterial)
            {
                currentTexture = _materialsPreview->GetPreview(_currentMaterial).RenderTex->GetColorTexture(0);
                if (currentTexture && currentTexture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_2D)
                {
                    ImGuiExt::RenderImage(currentTexture, 0, Vector2(26.0f, 26.0f));
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

                if (previewZoneWidth > 256.0f)
                    previewOffset = (previewZoneWidth - previewWidth) * 0.5f;

                ImGui::BeginChild("TexturePreview", ImVec2(previewZoneWidth, previewHeight), true, ImGuiWindowFlags_NoScrollbar);
                ImGuiExt::RenderImage(currentTexture, 0, Vector2(previewWidth - 16.0f, previewHeight - 16.0f), Vector2(previewOffset, 0.0f));
                ImGui::EndChild();

                ImVec2 cursor = ImGui::GetCursorPos();
                cursor.y -= 7.0f;
                ImGui::SetCursorPos(cursor);

                if (ShowPreviewButton(ICON_FA_CIRCLE, [this]() { 
                    return _materialsPreview->GetMeshPreviewType() != MaterialsPreview::MeshPreviewType::Sphere; }))
                {
                    _materialsPreview->SetMeshPreviewType(MaterialsPreview::MeshPreviewType::Sphere);
                }
                ImGui::SameLine();

                if (ShowPreviewButton(ICON_FA_CUBE, [this]() {
                    return _materialsPreview->GetMeshPreviewType() != MaterialsPreview::MeshPreviewType::Box; }))
                {
                    _materialsPreview->SetMeshPreviewType(MaterialsPreview::MeshPreviewType::Box);
                }
                ImGui::SameLine();

                if (ShowPreviewButton(ICON_FA_SQUARE, [this]() {
                    return _materialsPreview->GetMeshPreviewType() != MaterialsPreview::MeshPreviewType::Plane; }))
                {
                    _materialsPreview->SetMeshPreviewType(MaterialsPreview::MeshPreviewType::Plane);
                }

                cursor.x += 5.0f;
                ImGui::SetCursorPos(cursor);

                cursor.y += 40.0f;
                ImGui::SetCursorPos(cursor);
            }

            if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // TODO PBR

                {
                    Vector4 color = properties.BaseColor.GetAsVector4();
                    if (ImGuiExt::RenderColorRGB(color, "##material_prop_base_color_option", "Base Color", width))
                    {
                        hasChanged = true;
                        properties.BaseColor = Color(color);
                    }
                }
                ImGui::Separator();
                {
                    Vector4 color = properties.Emissive.GetAsVector4();
                    if (ImGuiExt::RenderColorRGB(color, "##material_prop_emissive_option", "Emissive", width))
                    {
                        hasChanged = true;
                        properties.Emissive = Color(color);
                    }
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Metallic, "##material_prop_metallic_option", "Metallic", 0.0f, 1.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Roughness, "##material_prop_roughness_option", "Roughness", 0.0f, 1.0f, width))
                        hasChanged = true;
                }
                ImGui::Separator();

                {
                    if (ImGuiExt::RenderOptionFloat(properties.Reflectance, "##material_prop_reflectance_option", "Reflectance", 0.0f, 1.0f, width))
                        hasChanged = true;
                }

                ImGui::Separator();
                {
                    if (ImGuiExt::RenderOptionBool(properties.UseIndirectLighting, "##material_properties_ibl_option", "Use Indirect Lighting"))
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

                // TODO PBR

                if (ShowTexture(uuid, properties.UseDiffuseIrrMap, "##material_texture_diffuse_irradiance_option", "Diff. Irradiance", "DiffuseIrrMap", texturesEnvMappingOptions, width, !properties.UseIndirectLighting, TextureType::TEX_TYPE_CUBE_MAP))
                    hasChanged = true;

                if (ShowTexture(uuid, properties.UseSpecularIrrMap, "##material_texture_specular_irradiance_option", "Spec. Irradiance", "SpecularIrrMap", texturesEnvMappingOptions, width, !properties.UseIndirectLighting, TextureType::TEX_TYPE_CUBE_MAP))
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
                    }

                    if (_currentMaterial->GetSamplerState("TextureSampler") == gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic))
                        samplerType = BuiltinSampler::Anisotropic;
                    else if (_currentMaterial->GetSamplerState("TextureSampler") == gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Trilinear))
                        samplerType = BuiltinSampler::Trilinear;
                    else
                        samplerType = BuiltinSampler::Bilinear;

                    if (ImGuiExt::RenderOptionCombo<BuiltinSampler>(&samplerType, "##material_sampler_type_option", "Sampler", samplerTypeOptions, width))
                    {
                        _currentMaterial->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(samplerType));
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

                _materialsPreview->MarkDirty(_currentMaterial);
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

        if (_fileBrowser.ShowFileDialog("Load Material Texture", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, ".png,.jpeg,.jpg,.dds,.tiff,.tga"))
        {
            auto textureImportOptions = TextureImportOptions::Create();
            if (_fileBrowser.Data.TexParam.TexType == TextureType::TEX_TYPE_CUBE_MAP)
            {
                textureImportOptions->CpuCached = _fileBrowser.Data.TexParam.CpuCached;
                textureImportOptions->CubemapType = CubemapSourceType::Faces;
                textureImportOptions->IsCubemap = true;
                textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
                textureImportOptions->SRGB = _fileBrowser.Data.TexParam.SRGB;
            }
            else
            {
                textureImportOptions->CpuCached = _fileBrowser.Data.TexParam.CpuCached;
                textureImportOptions->GenerateMips = _fileBrowser.Data.TexParam.GenerateMips;
                textureImportOptions->MaxMip = _fileBrowser.Data.TexParam.MaxMips;
                textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
                textureImportOptions->SRGB = _fileBrowser.Data.TexParam.SRGB;
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

        _materialsPreview->DeletePreview(material);

        EditorResManager::Instance().Remove<Material>(handle);
        material = nullptr;
        _currentMaterial = nullptr;
        gEditor().NeedsRedraw();
    }
}
