#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetProject : public Widget
    {
    public:
        WidgetProject();
        ~WidgetProject();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        void ShowTree(const HSceneObject& sceneObject, bool expand = false);
        void ShowComponentTree(const HSceneObject& sceneObject);
    };
}
