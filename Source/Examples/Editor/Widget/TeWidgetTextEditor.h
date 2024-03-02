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

    public:
        Event<void()> OnBuild;
        Event<void()> OnSave;

    private:
        void Save(bool force = false);
        void Build();
        void UpdateEditorContent();
        void ShowEditor();
        void ShowToolbar();

    private:
        Editor::SelectionData& _selections;
        SPtr<ImGuiTextEditor> _editor;
        String _editorContent;
        bool _needsSave;
    };
}
