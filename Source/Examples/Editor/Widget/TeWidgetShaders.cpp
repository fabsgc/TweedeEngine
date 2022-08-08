#include "TeWidgetShaders.h"

#include "../TeEditor.h"
#include "../TeEditorResManager.h"
#include "../ImGuiExt/TeImGuiExt.h"
#include "../MaterialsPreview/TeMaterialsPreview.h"
#include "Material/TeShader.h"
#include "Material/TeMaterial.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    WidgetShaders::WidgetShaders()
        : Widget(WidgetType::Shaders)
    { 
        _title = SHADERS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetShaders::~WidgetShaders()
    { }

    void WidgetShaders::Initialize()
    { }

    void WidgetShaders::Update()
    {
        bool hasChanged = false;
        UUID empty = UUID(50, 0, 0, 0);
        ImGuiExt::ComboOptions<UUID> shadersOptions;
        UUID shaderUUID = (_currentShader) ? _currentShader->GetUUID() : empty;
        EditorResManager::ResourcesContainer& shaders = EditorResManager::Instance().Get<Shader>();
        EditorResManager::ResourcesContainer& materials = EditorResManager::Instance().Get<Material>();
        const float width = ImGui::GetWindowContentRegionWidth() - 110.0f;

        // Shaders list
        {
            for (auto& resource : shaders.Res)
            {
                if (!_currentShader)
                {
                    _currentShader = gResourceManager().Load<Shader>(resource.second->GetUUID()).GetInternalPtr();
                    shaderUUID = _currentShader->GetUUID();
                }

                shadersOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());
            }

            if (_currentShader)
            {
                if (ImGuiExt::RenderOptionCombo<UUID>(&shaderUUID, "##shader_list_option", "",
                    shadersOptions, ImGui::GetWindowContentRegionWidth()))
                {
                    if (shaderUUID != _currentShader->GetUUID())
                        _currentShader = gResourceManager().Load<Shader>(shaderUUID).GetInternalPtr();
                }

                // Built Shader
                {
                    ImGui::PushID("##shader_build_option");
                    if (ImGui::Button(ICON_FA_SCREWDRIVER " Build Shader", ImVec2(ImGui::GetWindowContentRegionWidth(), 25.0f)))
                    {
                        Build();
                        hasChanged = true;
                    }
                    ImGui::PopID();
                }

                // Identification
                {
                    char inputName[256];
                    char inputUUID[64];

                    String name = _currentShader->GetName();
                    String uuidStr = _currentShader->GetUUID().ToString();

                    memset(&inputName, 0, 256);
                    strcpy(inputUUID, uuidStr.c_str());

                    if (name.length() < 256) strcpy(inputName, name.c_str());
                    else strcpy(inputName, name.substr(0, 255).c_str());

                    if (ImGui::CollapsingHeader("Identification"))
                    {
                        ImGui::PushItemWidth(width);
                        if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
                            _currentShader->SetName(inputName);
                        ImGui::Separator();

                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
                        ImGui::PopItemFlag();
                        ImGui::PopItemWidth();
                    }
                }

                if (hasChanged)
                {
                    gEditor().NeedsRedraw();
                    gEditor().GetSettings().State = Editor::EditorState::Modified;

                    for (auto& resource : materials.Res)
                    {
                        auto material = static_resource_cast<Material>(resource.second);
                        if (material.IsLoaded() && material->GetShader() == _currentShader)
                        {
                            gEditor().GetMaterialsPreview()->MarkDirty(material.GetInternalPtr());
                        }
                    }
                }
            }
        }
    }

    void WidgetShaders::Build()
    {
        if (!_currentShader)
            return;

        _currentShader->Compile(true);
    }

    void WidgetShaders::UpdateBackground()
    { }
}
