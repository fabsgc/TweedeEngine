#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    struct HINGE_JOINT_DESC;

    /**
     * Hinge joint removes all but a single rotation degree of freedom from its two attached bodies (for example a door
     * hinge).
     */
    class TE_CORE_EXPORT HingeJoint : public Joint
    {
    public:
        HingeJoint(const HINGE_JOINT_DESC& desc) { }
        virtual ~HingeJoint() = default;
    };

    /** Structure used for initializing a new HingeJoint. */
    struct HINGE_JOINT_DESC
    { };
}
