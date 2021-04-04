#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Math/TeVector3.h"

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

        /** Sets the scale of the collider geometry. */
        virtual void SetScale(const Vector3& scale);

        /** Retrieves the scale of the collider geometry. */
        Vector3 GetScale() const;

        /** @copydoc FCollider::SetIsTrigger */
        void SetIsTrigger(bool value);

        /** @copydoc FCollider::GetIsTrigger */
        bool GetIsTrigger() const;

        /** Determines the Rigidbody that controls this collider (if any). */
        void SetRigidBody(RigidBody* value);

        /** @copydoc Collider::setRigidbody() */
        RigidBody* GetRigidbody() const { return _rigidBody; }

        /** Returns the object containing common collider code. */
        FCollider* GetInternal() const { return _internal; }

    protected:
        FCollider* _internal = nullptr;
        RigidBody* _rigidBody = nullptr;
        Vector3 _scale = Vector3::ONE;
    };
}
