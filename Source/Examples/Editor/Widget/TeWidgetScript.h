#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"
#include "ImGui/imgui.h"

namespace te
{
    class ImGuiTextEditor;

    class WidgetScript : public Widget
    {
    public:
        WidgetScript();
        ~WidgetScript();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
        virtual void PutFocus() override;

    private:
        void Save(bool force = false);
        void Build();
        void UpdateEditorContent();
        void ShowEditor();
        void ShowToolbar();

    private:
        Editor::SelectionData& _selections;
        SPtr<ImGuiTextEditor> _editor;

        String _currentScriptName;
        String _currentScriptPath;
        String _editorContent;
        bool _needsUpdateContent;
        bool _needsSave;
    };
}
