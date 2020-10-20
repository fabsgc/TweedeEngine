#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

#include <unordered_map>

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
        std::unordered_map<Widget::IconType, SPtr<Widget>> _widgets;
        float _buttonSize = 30.0f;
    };
}
