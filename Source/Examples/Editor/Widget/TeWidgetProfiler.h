#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetProfiler : public Widget
    {
    public:
        WidgetProfiler();
        ~WidgetProfiler();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
