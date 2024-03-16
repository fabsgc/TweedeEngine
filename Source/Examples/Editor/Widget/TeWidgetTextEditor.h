#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"

namespace te
{
    class ImGuiTextEditor;


    class WidgetTextEditor : public Widget
    {
    public:
        WidgetTextEditor();
        virtual ~WidgetTextEditor();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
        virtual void PutFocus() override;

        void SetScript(Script* script);
        const Script* GetScript() const;

    public:
        Event<void()> OnBuild;
        Event<void()> OnSave;

    private:
        void UpdateEditorContent();
        void ShowEditor();
        void ShowToolbar();

    private:
        Editor::SelectionData& _selections;
        SPtr<ImGuiTextEditor> _editor;
        String _editorContent;
        bool _needsSave;

        Script* _script = nullptr;
    };
}
