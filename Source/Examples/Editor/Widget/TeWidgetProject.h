#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"

namespace te
{
    class WidgetProject : public Widget
    {
    public:
        WidgetProject();
        virtual ~WidgetProject();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
