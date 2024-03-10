#include "TeWidgetScripts.h"

#include "../TeEditor.h"
#include "../TeEditorResManager.h"
#include "../ImGuiExt/TeImGuiExt.h"
#include "../ImGuiExt/TeImGuiFileBrowser.h"
#include "Scene/TeSceneManager.h"
#include "Scripting/TeScript.h"

namespace te
{
    WidgetScripts::WidgetScripts()
        : Widget(WidgetType::Scripts)
        , _currentScript(nullptr)
        , _scriptCreationCounter(1)
        , _fileBrowser(gEditor().GetFileBrowser())
    { 
        _title = SCRIPTS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetScripts::~WidgetScripts()
    { }

    void WidgetScripts::Initialize()
    {
        _currentScript = nullptr;
    }

    void WidgetScripts::Update()
    {
        bool hasChanged = false;
        char inputPath[1024];
        char inputName[256];
        char inputUUID[64];
        UUID load = UUID::EMPTY;
        UUID empty = UUID(50, 0, 0, 0);
        float offsetListScripts = 0.0f;
        ImGuiExt::ComboOptions<UUID> scriptsOptions;
        UUID scriptUUID = (_currentScript) ? _currentScript->GetUUID() : empty;
        EditorResManager::ResourcesContainer& scripts = EditorResManager::Instance().Get<Script>();
        const float width = ImGui::GetWindowContentRegionWidth() - 110.0f;

        // New script
        {
            ImGui::PushID("##script_new_option");
            if (ImGui::Button(ICON_FA_PLUS_SQUARE " New script", ImVec2(ImGui::GetWindowContentRegionWidth(), 25.0f)))
            {
                HScript script = Script::Create();
                script->SetName("Script " + ToString(_scriptCreationCounter));
                EditorResManager::Instance().Add(script);
                _currentScript = script.GetInternalPtr();
                _scriptCreationCounter++;
                scriptUUID = _currentScript->GetUUID();
            }
            ImGui::PopID();
        }

        // Scripts list
        {
            for (auto& resource : scripts.Res)
            {
                if (!_currentScript)
                {
                    _currentScript = gResourceManager().Load<Script>(resource.second->GetUUID()).GetInternalPtr();
                    scriptUUID = _currentScript->GetUUID();
                }

                scriptsOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());
            }

            if (_currentScript)
            {
                if (ImGuiExt::RenderOptionCombo<UUID>(&scriptUUID, "##script_list_option", "", 
                    scriptsOptions, ImGui::GetWindowContentRegionWidth() - 32.0f - offsetListScripts))
                {
                    if (scriptUUID != _currentScript->GetUUID())
                        _currentScript = gResourceManager().Load<Script>(scriptUUID).GetInternalPtr();
                }

                // You can delete a script
                // Be careful this feature also reset all components that are using this script
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_TIMES_CIRCLE, ImVec2(25.0f, 26.0f)))
                {
                    DeleteScript(_currentScript, scriptUUID);
                }
            }
        }

        // Current script properties
        if(_currentScript)
        {
            String name = _currentScript->GetName();
            String uuidStr = _currentScript->GetUUID().ToString();

            memset(&inputName, 0, 256);
            strcpy(inputUUID, uuidStr.c_str());

            if (name.length() < 256) strcpy(inputName, name.c_str());
            else strcpy(inputName, name.substr(0, 256).c_str());

            if (ImGui::CollapsingHeader("Identification"))
            {
                ImGui::PushItemWidth(width);
                if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
                    _currentScript->SetName(inputName);
                ImGui::Separator();

                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
                ImGui::PopItemFlag();
                ImGui::PopItemWidth();
            }

            if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
            {
                String path = _currentScript->GetPath();
                if (path.length() < 1024) strcpy(inputPath, path.c_str());
                else strcpy(inputPath, name.substr(0, 1024).c_str());

                ImGui::PushItemWidth(width + 45.0f);
                ImGui::InputText("Path", inputPath, IM_ARRAYSIZE(inputPath), ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_FOLDER_OPEN, ImVec2(28.0f, 26.0f)))
                {
                    _loadScript = true;
                }
            }

            if (ShowLoadScript())
                hasChanged = true;
        }

        if (hasChanged)
        {

        }
    }

    void WidgetScripts::UpdateBackground()
    { }

    void WidgetScripts::DeleteScript(SPtr<Script> script, const UUID& uuid)
    {
        HScript handle = static_resource_cast<Script>(
            gResourceManager()._createResourceHandle(script, uuid));

        Vector<HSceneObject> sceneObjects = gSceneManager().GetRootNode()->GetSceneObjects(true); 

        for (auto& sceneObject : sceneObjects)
        {
            sceneObject->RemoveScript(handle);
        }

        EditorResManager::Instance().Remove<Script>(handle);
        script = nullptr;
        _currentScript = nullptr;
        gEditor().NeedsRedraw();
    }

    bool WidgetScripts::ShowLoadScript()
    {
        bool scriptLoaded = false;

        if (_loadScript)
            ImGui::OpenPopup("Load Script");

        if (_fileBrowser.ShowFileDialog("Load Script", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, Editor::ScriptsExtensionsStr))
        {
            scriptLoaded = true;
            _loadScript = false;

            _currentScript->SetPath(_fileBrowser.Data.SelectedPath);
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
            {
                _loadScript = false;
            }
        }

        return scriptLoaded;
    }
}
