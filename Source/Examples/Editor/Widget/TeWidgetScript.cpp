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

            if (name.empty())
                return;

            if (name != _currentScriptName)
            {
                _currentScriptName = name;
                _needsUpdateContent = true;
            }

            if (_needsUpdateContent)
            {
#if TE_PLATFORM == TE_PLATFORM_WIN32
                static String appRoot = ReplaceAll(RAW_APP_ROOT, "/", "\\");
                static String librariesPath = ReplaceAll(ScriptManager::LIBRARIES_PATH, "/", "\\");
                String path = appRoot + librariesPath + name + ".cpp ";
#else
                String path = RAW_APP_ROOT + librariesPath + name + ".cpp ";
#endif
                FileStream file(path);
                _editorContent = file.GetAsString();
                _needsUpdateContent = false;
            }
        }
        else if(!ImGui::IsWindowFocused())
        {
            _needsUpdateContent = true;
        }
    }

    void WidgetScript::UpdateBackground()
    {
        _needsUpdateContent = true;
    }
}
