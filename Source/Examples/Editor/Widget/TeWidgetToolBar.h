#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetToolBar : public Widget
    {
    public:
        WidgetToolBar();
        virtual ~WidgetToolBar();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        float _buttonSize = 30.0f;
    };
}
