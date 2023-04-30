#include "TeWidgetShaders.h"

#include "../TeEditor.h"
#include "../TeEditorResManager.h"
#include "../ImGuiExt/TeImGuiExt.h"
#include "../MaterialsPreview/TeMaterialsPreview.h"
#include "Material/TeShader.h"
#include "Material/TeMaterial.h"
#include "Scene/TeSceneObject.h"
#include "Renderer/TeRendererMaterialManager.h"

namespace te
{
    WidgetShaders::WidgetShaders()
        : Widget(WidgetType::Shaders)
        , _showTechniques(false)
    { 
        _title = SHADERS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetShaders::~WidgetShaders()
    { }

    void WidgetShaders::Initialize()
    { 
        HShader shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque);
        if (shader.IsLoaded())
            _currentShader = shader.GetInternalPtr();
    }

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

                ImGuiExt::RenderOptionBool(_showTechniques, "show_techniques", "Show Techniques");

                // Techniques
                if (_showTechniques)
                {
                    UINT32 i = 1, j = 1;

                    const auto& techniques =_currentShader->GetTechniques();
                    for (auto& technique : techniques)
                    {
                        ImGui::PushID(i);
                        if (ImGui::CollapsingHeader("Technique"))
                        {
                            char inputLanguage[32];
                            const String& language = technique->GetLanguage();

                            if (language.length() < 256)
                                strcpy(inputLanguage, language.c_str());
                            else
                                strcpy(inputLanguage, language.substr(0, 255).c_str());

                            ImGui::PushItemWidth(width);
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                            ImGui::InputText("Language", inputLanguage, IM_ARRAYSIZE(inputLanguage));
                            ImGui::PopItemFlag();
                            ImGui::PopItemWidth();

                            ImGui::PushID("Technique Content");
                            if (ImGui::CollapsingHeader("Variation"))
                            {
                                ShaderVariation shaderVariation = technique->GetVariation();
                                for (auto& param : shaderVariation.GetParams())
                                {
                                    String value;
                                    char inputValue[32];

                                    if (param.second.Type == ShaderVariation::ParamType::Boolean)
                                        value += ToString((INT32)param.second.I);
                                    else if (param.second.Type == ShaderVariation::ParamType::Float)
                                        value += ToString((float)param.second.F);
                                    else if (param.second.Type == ShaderVariation::ParamType::Int)
                                        value += ToString((INT32)param.second.I);
                                    else if (param.second.Type == ShaderVariation::ParamType::UInt)
                                        value += ToString((UINT32)param.second.Ui);

                                    if (value.length() < 256)
                                        strcpy(inputValue, value.c_str());
                                    else
                                        strcpy(inputValue, value.substr(0, 255).c_str());

                                    ImGui::PushItemWidth(width - 25);
                                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                                    ImGui::InputText(param.second.Name.c_str(), inputValue, IM_ARRAYSIZE(inputValue));
                                    ImGui::PopItemFlag();
                                    ImGui::PopItemWidth();
                                }
                            }

                            j = 1;
                            ImGui::PushID("Passes content");
                            /*for (auto& pass : technique->GetPasses())
                            {
                                ImGui::PushID(j);
                                if (ImGui::CollapsingHeader("Pass"))
                                {

                                }
                                ImGui::PopID();

                                j++;
                            }*/
                            ImGui::PopID();
                            ImGui::PopID();
                        }
                        ImGui::PopID();

                        i++;
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
                            gEditor().GetMaterialsPreview().MarkDirty(material.GetInternalPtr());
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

        Vector<RendererMaterialData>& materials = RendererMaterialManager::GetMaterials();
        for (auto& material : materials)
        {
            if (material.MetaData->ShaderElem == _currentShader)
            {
                for(auto& instance : material.MetaData->Instances)
                {
                    instance->InitPipelines();
                    instance->Initialize();
                }
            }
        }
    }

    void WidgetShaders::UpdateBackground()
    { }
}
