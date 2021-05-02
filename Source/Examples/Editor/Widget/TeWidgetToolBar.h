#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetToolBar : public Widget
    {
    public:
        WidgetToolBar();
        ~WidgetToolBar();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        UnorderedMap<Widget::IconType, SPtr<Widget>> _widgets;
        float _buttonSize = 30.0f;
    };
}
