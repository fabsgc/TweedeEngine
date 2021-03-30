#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    struct CONE_TWIST_JOINT_DESC;

    /**
     * To create ragdolls, the conve twist constraint is very useful for limbs like the upper arm. It is a special 
     * point to point constraint that adds cone and twist axis limits. The x-axis serves as twist axis.
     */
    class TE_CORE_EXPORT ConeTwistJoint : public Joint
    {
    public:
        ConeTwistJoint(const CONE_TWIST_JOINT_DESC& desc) { }
        virtual ~ConeTwistJoint() = default;
    };

    /** Structure used for initializing a new ConeTwistJoint. */
    struct CONE_TWIST_JOINT_DESC
    { };
}
