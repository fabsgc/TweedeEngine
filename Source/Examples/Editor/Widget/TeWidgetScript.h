#pragma once

#include "TeCorePrerequisites.h"
#include "../ImGuiExt/TeImGuiTextEditor.h"
#include "../TeEditor.h"
#include "TeWidget.h"
#include "ImGui/imgui.h"

namespace te
{
    class WidgetScript : public Widget
    {
    public:
        WidgetScript();
        ~WidgetScript();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

        void SaveAndCompile();

    private:
        Editor::SelectionData& _selections;
        ImGuiTextEditor _editor;

        String _currentScriptName;
        String _currentScriptPath;
        String _editorContent;
        bool _needsUpdateContent;
        bool _needsSave;
    };
}
