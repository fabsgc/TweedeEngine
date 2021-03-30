#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"

namespace te
{
    struct JOINT_DESC;

    /**
     * Base class for all Joint types. Joints constrain how two rigidbodies move relative to one another (for example a door
     * hinge). One of the bodies in the joint must always be movable (non-kinematic).
     */
    class TE_CORE_EXPORT Joint
    {
    public:
        Joint() = default;
        virtual ~Joint() = default;
    };

    /** Structure used for initializing a new Joint. */
    struct JOINT_DESC
    { };
}
