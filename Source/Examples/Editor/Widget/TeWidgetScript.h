#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"

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

    private:
        Editor::SelectionData& _selections;
        String _currentScriptName;
        String _editorContent;
        bool _needsUpdateContent;
    };
}
