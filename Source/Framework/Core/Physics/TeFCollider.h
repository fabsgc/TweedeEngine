#include "Physics/TePhysicsCommon.h"

namespace te
{
    /** Provides common functionality used by all Collider types. */
    class TE_CORE_EXPORT FCollider
    {
    public:
        virtual ~FCollider() = default;

        /**
         * Enables/disables a collider as a trigger. A trigger will not be used for collisions (objects will pass
         * through it), but collision events will still be reported.
         */
        virtual void SetIsTrigger(bool value) = 0;

        /** @copydoc GetIsTrigger() */
        virtual bool GetIsTrigger() const = 0;

        /**
         * Determines whether the collider is a part of a rigidbody (non-static), or is on its own (static). You should
         * change this whenever you are attaching or detaching a collider from a rigidbody.
         */
        virtual void SetIsStatic(bool value) = 0;

        /** @copydoc GetIsStatic() */
        virtual bool GetIsStatic() const = 0;

    };
}
