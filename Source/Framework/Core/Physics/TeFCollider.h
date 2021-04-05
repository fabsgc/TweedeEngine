#include "Physics/TePhysicsCommon.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Provides common functionality used by all Collider types. */
    class TE_CORE_EXPORT FCollider
    {
    public:
        virtual ~FCollider() = default;

        /** Returns the position of the collider. */
        virtual Vector3 GetPosition() const = 0;

        /** Returns the rotation of the collider. */
        virtual Quaternion GetRotation() const = 0;

        /** Sets the position and rotation of the collider. */
        virtual void SetTransform(const Vector3& pos, const Quaternion& rotation) = 0;

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

        /** Determines which (if any) collision events are reported. */
        virtual void SetCollisionReportMode(CollisionReportMode mode) = 0;

        /** @copydoc setCollisionReportMode() */
        virtual CollisionReportMode GetCollisionReportMode() const = 0;

    };
}
