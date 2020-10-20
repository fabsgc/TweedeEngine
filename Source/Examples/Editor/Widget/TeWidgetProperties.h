#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetProperties : public Widget
    {
    public:
        WidgetProperties();
        ~WidgetProperties();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
