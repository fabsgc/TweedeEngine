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

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the Slider joint that this component wraps. */
        SliderJoint* GetInternal() const { return static_cast<SliderJoint*>(_internal.get()); }

    protected:
        CSliderJoint(); // Serialization only
    };
}
