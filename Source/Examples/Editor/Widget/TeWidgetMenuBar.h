#pragma once

#include "TeCorePrerequisites.h"
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

        void ShowAboutWindow();

    protected:
        struct WidgetMenubarSettings
        {
            bool ShowAboutWindow = false;
        };

    protected:
        WidgetMenubarSettings _settings;
    };
}
