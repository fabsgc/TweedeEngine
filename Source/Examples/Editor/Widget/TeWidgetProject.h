#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetProject : public Widget
    {
    public:
        WidgetProject();
        ~WidgetProject();

        virtual void Initialize() override;
        virtual void Update() override;
    };
}
