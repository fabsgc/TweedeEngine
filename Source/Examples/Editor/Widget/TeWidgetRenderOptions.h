#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetRenderOptions : public Widget
    {
    public:
        struct RenderOptions
        {

        };

    public:
        WidgetRenderOptions();
        ~WidgetRenderOptions();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        SPtr<Renderer> _renderer;
    };
}
