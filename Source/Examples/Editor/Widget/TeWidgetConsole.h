#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetConsole : public Widget
    {
    public:
        WidgetConsole();
        ~WidgetConsole();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
