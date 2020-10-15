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
    };
}
