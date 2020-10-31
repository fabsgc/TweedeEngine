#pragma once

#include "TeCorePrerequisites.h"
#include "Input/TeVirtualInput.h"
#include "TeWidget.h"
#include "../ImGuiExt/TeImGuiFileBrowser.h"

namespace te
{
    class WidgetMenuBar : public Widget
    {
    public:
        WidgetMenuBar();
        ~WidgetMenuBar();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        void ShowAboutWindow();
        void ShowOpen();
        void ShowSave();

    protected:
        struct WidgetMenubarSettings
        {
            bool ShowAboutWindow = false;
        };

    protected:
        WidgetMenubarSettings _settings;
        ImGuiFileBrowser& _fileBrowser;

        bool _open = false;
        bool _save = false;

        VirtualButton _newBtn;
        VirtualButton _openBtn;
        VirtualButton _saveBtn;
        VirtualButton _saveAsBtn;
        VirtualButton _quitBtn;
    };
}
