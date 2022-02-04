#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetProfiler : public Widget
    {
    public:
        WidgetProfiler();
        virtual ~WidgetProfiler();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
