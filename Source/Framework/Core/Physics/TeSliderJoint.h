#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    struct SLIDER_JOINT_DESC;

    /**
     * Joint that removes all but a single translational degree of freedom. Bodies are allowed to move along a single axis.
     */
    class TE_CORE_EXPORT SliderJoint : public Joint
    {
    public:
        SliderJoint(const SLIDER_JOINT_DESC& desc) { }
        virtual ~SliderJoint() = default;

        /**
         * Creates a new slider joint.
         *
         * @param[in]	scene		Scene to which to add the joint.
         * @param[in]	desc		Settings describing the joint.
         */
        static SPtr<SliderJoint> Create(PhysicsScene& scene, const SLIDER_JOINT_DESC& desc);
    };

    /** Structure used for initializing a new SliderJoint. */
    struct SLIDER_JOINT_DESC : JOINT_DESC
    { };
}
