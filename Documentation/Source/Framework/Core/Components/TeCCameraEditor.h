#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Math/TeMath.h"
#include "Input/TeVirtualInput.h"

namespace te
{
    /** Component that controls movement and rotation of the scene object it's attached to. */
    class TE_CORE_EXPORT CCameraEditor : public Component
    {
    public:
        CCameraEditor(const HSceneObject& parent);

        /** Triggered once per frame. Allows the component to handle input and move. */
        void Update() override;
    };
}
