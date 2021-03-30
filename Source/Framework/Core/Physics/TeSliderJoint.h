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
    };

    /** Structure used for initializing a new SliderJoint. */
    struct SLIDER_JOINT_DESC
    { };
}
