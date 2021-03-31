#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSliderJoint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	SliderJoint
     *
     * @note	Wraps SliderJoint as a Component.
     */
    class TE_CORE_EXPORT CSliderJoint : public CJoint
    {
    public:
        CSliderJoint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CSliderJoint; }

        /** @copydoc Component::Clone */
        void Clone(const HSliderJoint& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        SLIDER_JOINT_DESC _desc;

    protected:
        CSliderJoint(); // Serialization only
    };
}
