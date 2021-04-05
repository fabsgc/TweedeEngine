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

        /**
         * Sets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        void SetOwner(PhysicsOwnerType type) { _owner.Type = type; }

        /**
         * Gets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        PhysicsOwnerType GetOwner() const { return _owner.Type; }

    protected:
        PhysicsObjectOwner _owner;
        FJoint* _internal = nullptr;
    };

    /** Structure used for initializing a new Joint. */
    struct JOINT_DESC
    { };
}
