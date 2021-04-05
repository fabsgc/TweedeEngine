#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Math/TeVector3.h"
#include "Utility/TeEvent.h"

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

        /** @copydoc FCollider::GetPosition */
        Vector3 GetPosition() const;

        /** @copydoc FCollider::GetRotation */
        Quaternion GetRotation() const;

        /** @copydoc FCollider::SetTransform */
        void SetTransform(const Vector3& pos, const Quaternion& rot);

        /** Sets the scale of the collider geometry. */
        virtual void SetScale(const Vector3& scale);

        /** Retrieves the scale of the collider geometry. */
        Vector3 GetScale() const;

        /** @copydoc FCollider::SetIsTrigger */
        void SetIsTrigger(bool value);

        /** @copydoc FCollider::GetIsTrigger */
        bool GetIsTrigger() const;

        /** @copydoc FCollider::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode);

        /** @copydoc FCollider::getCollisionReportMode */
        CollisionReportMode GetCollisionReportMode() const;

        /** Determines the Rigidbody that controls this collider (if any). */
        void SetRigidBody(RigidBody* value);

        /** @copydoc Collider::setRigidbody() */
        RigidBody* GetRigidbody() const { return _rigidBody; }

        /** Returns the object containing common collider code. */
        FCollider* GetInternal() const { return _internal; }

        /**
         * Sets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        void SetOwner(PhysicsOwnerType type, void* owner) { _owner.Type = type; _owner.OwnerData = owner; }

        /**
         * Gets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        void* GetOwner(PhysicsOwnerType type) const { return _owner.Type == type ? _owner.OwnerData : nullptr; }

        /**
         * Triggered when some object starts interacting with the collider. Only triggered if proper collision report mode
         * is turned on.
         */
        Event<void(const CollisionDataRaw&)> OnCollisionBegin;
        /**
         * Triggered for every frame that an object remains interacting with a collider. Only triggered if proper collision
         * report mode is turned on.
         */
        Event<void(const CollisionDataRaw&)> OnCollisionStay;
        /**
         * Triggered when some object stops interacting with the collider. Only triggered if proper collision report mode
         * is turned on.
         */
        Event<void(const CollisionDataRaw&)> OnCollisionEnd;

    protected:
        FCollider* _internal = nullptr;
        PhysicsObjectOwner _owner;
        RigidBody* _rigidBody = nullptr;
        Vector3 _scale = Vector3::ONE;
    };
}
