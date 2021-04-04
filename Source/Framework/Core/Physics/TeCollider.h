#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"

namespace te
{
    /**
     * Collider represents physics geometry that can be in multiple states:
     *  - Default: Static geometry that physics objects can collide with.
     *  - Trigger: Static geometry that can't be collided with but will report touch events.
     *  - Dynamic: Dynamic geometry that is a part of a Rigidbody. A set of colliders defines the shape of the parent
     *		       rigidbody.
     */
    class TE_CORE_EXPORT Collider
    {
    public:
        Collider() = default;
        virtual ~Collider() = default;
    };
}
