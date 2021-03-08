#include "TeWidgetScript.h"
#include "../TeEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "TeCoreApplication.h"
#include "Gui/TeGuiAPI.h"
#include "Components/TeCScript.h"
#include "Scripting/TeScriptManager.h"
#include "Utility/TeFileStream.h"

namespace te
{
    WidgetScript::WidgetScript()
        : Widget(WidgetType::Script)
        , _selections(gEditor().GetSelectionData())
        , _needsUpdateContent(false)
        , _needsSave(false)
    {
        _title = SCRIPT_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove;
    }

    WidgetScript::~WidgetScript()
    { }

    void WidgetScript::Initialize()
    { }

    void WidgetScript::Update()
    {
        if (_selections.ClickedComponent && _selections.ClickedComponent->GetCoreType() == TID_CScript)
        {
            SPtr<CScript> script = std::static_pointer_cast<CScript>(_selections.ClickedComponent);
            String name = script->GetNativeScriptName();
            String path = script->GetNativeScriptPath();

            if (name.empty())
                return;

            if (name != _currentScriptName) // Update current script name
            {
                _currentScriptName = name;
                _currentScriptPath = path;
                _needsUpdateContent = true;
            }

            if (_needsUpdateContent) // If current script name different from previous, we need to load its content
            {
                FileStream file(path + _currentScriptName + ".cpp"); // TODO
                _editorContent = file.GetAsString();
                file.Close();

                _needsUpdateContent = false;
                _needsSave = true;
            }
        }
        else if(!ImGui::IsWindowFocused())
        {
            SaveAndCompile();
            _needsUpdateContent = true;
        }
    }

    void WidgetScript::UpdateBackground()
    {
        SaveAndCompile();
        _needsUpdateContent = true;
    }

    void WidgetScript::SaveAndCompile()
    {
        if (_needsUpdateContent == false && !_currentScriptName.empty() && _needsSave)
        {
            String path = _currentScriptPath + _currentScriptName + ".cpp"; // TODO
            FileStream file(path);
            file.Write((void*)(_editorContent.c_str()), _editorContent.size());
            file.Close();

            gScriptManager().OnMonitorFileModified(path);

            _needsSave = false;
        }
    }
}
