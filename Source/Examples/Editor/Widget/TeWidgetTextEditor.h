#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"

namespace te
{
    class ImGuiTextEditor;


    class WidgetScriptEditor : public Widget
    {
    public:
        WidgetScriptEditor();
        virtual ~WidgetScriptEditor();

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
        UnorderedMap<String, String> _existingScripts; //Name - Path
        String _currentScriptName;
        String _editorContent;
        bool _needsSave;
    };
}
