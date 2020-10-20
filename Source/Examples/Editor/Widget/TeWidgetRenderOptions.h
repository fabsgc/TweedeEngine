#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetRenderOptions : public Widget
    {
    public:
        WidgetRenderOptions();
        ~WidgetRenderOptions();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
