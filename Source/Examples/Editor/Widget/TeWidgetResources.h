#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetResources : public Widget
    {
    public:
        WidgetResources();
        ~WidgetResources();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;
    };
}
