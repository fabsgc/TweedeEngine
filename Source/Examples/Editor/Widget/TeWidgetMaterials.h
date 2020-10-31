#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"

namespace te
{
    class WidgetMaterials : public Widget
    {
    public:
        WidgetMaterials();
        ~WidgetMaterials();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        SPtr<Material> _currentMaterial;
        UINT32 _materialCreationCounter;
    };
}
