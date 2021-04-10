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

        /**
         * Enables/disables a collider as a trigger. A trigger will not be used for collisions (objects will pass
         * through it), but collision events will still be reported.
         */
        virtual void SetIsTrigger(bool value) = 0;

        /** @copydoc GetIsTrigger() */
        virtual bool GetIsTrigger() const = 0;

        /**
         * Determines the scale of the collider relative to its parent
         */
        virtual void SetScale(const Vector3& scale) { _scale = scale; }

        /** @copydoc SetScale() */
        virtual const Vector3& GetScale() const { return _scale; }

        /**
         * Determines the mass of the collider. Only relevant if the collider is part of a rigidbody. Ultimately this will
         * determine the total mass, center of mass and inertia tensors of the parent rigidbody (if they're being calculated
         * automatically).
         */
        virtual void SetMass(float mass) { _mass = mass; }

        /** @copydoc SetMass() */
        virtual float GetMass() const { return _mass; }

        /**
         * Determines the center of the collider relative to its parent
         */
        virtual void SetCenter(const Vector3& center) { _center = center; }

        /** @copydoc SetCenter() */
        virtual const Vector3& GetCenter() const { return _center; }

        /** Determines which (if any) collision events are reported. */
        virtual void SetCollisionReportMode(CollisionReportMode mode) = 0;

        /** @copydoc SetCollisionReportMode() */
        virtual CollisionReportMode GetCollisionReportMode() const = 0;

    protected:
        Vector3 _scale = Vector3(1.0f, 1.0f, 1.0f);
        float _mass = 1.0f;
        Vector3 _center = Vector3::ZERO;
    };
}
