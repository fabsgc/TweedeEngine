#pragma once

#include "TeCorePrerequisites.h"
#include "Input/TeVirtualInput.h"
#include "TeWidget.h"

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

        void ShowAboutWindow();

    protected:
        struct WidgetMenubarSettings
        {
            bool ShowAboutWindow = false;
        };

    protected:
        WidgetMenubarSettings _settings;
        VirtualButton _newBtn;
        VirtualButton _quitBtn;
    };
}
