#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetShaders : public Widget
    {
    public:
        WidgetShaders();
        virtual ~WidgetShaders();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    private:
        void Build();

    private:
        SPtr<Shader> _currentShader;
    };
}
