#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"

namespace te
{
    class WidgetScripts : public Widget
    {
    public:
        WidgetScripts();
        virtual ~WidgetScripts();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
