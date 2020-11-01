#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetRenderOptions : public Widget
    {
    public:
        WidgetRenderOptions();
        ~WidgetRenderOptions();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

        void CameraCreated(const HComponent& component);
        void CameraDestroyed(const HComponent& component);

    protected:
        ImGuiExt::ComboOptions<HCamera> _cameraList;
        HCamera _currentCamera;
        HEvent _cameraCreated;
        HEvent _cameraDestroyed;
    };
}
