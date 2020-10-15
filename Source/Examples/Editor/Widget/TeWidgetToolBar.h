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

    protected:
        std::unordered_map<Widget::IconType, SPtr<Widget>> _widgets;
        float _buttonSize = 25.0f;
    };
}
