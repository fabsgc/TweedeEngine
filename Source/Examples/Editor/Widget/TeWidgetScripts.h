#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class ImGuiFileBrowser;

    class WidgetScripts : public Widget
    {
    public:
        WidgetScripts();
        virtual ~WidgetScripts();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    private:
        void DeleteScript(SPtr<Script> script, const UUID& uuid);

    private:
        SPtr<Script> _currentScript;
        UINT32 _scriptCreationCounter;

        ImGuiFileBrowser& _fileBrowser;
    };
}
