#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Utility/TeEvent.h"

namespace te
{
    /**
     * Base class for all Body types.
     */
    class TE_CORE_EXPORT Body
    {
    public:
        Body(const HSceneObject& linkedSO);
        virtual ~Body() = default;

        /** Registers a new collider as a child of this rigidbody. */
        virtual void AddCollider(Collider* collider) = 0;

        /** Removes a collider from the child list of this rigidbody. */
        virtual void RemoveCollider(Collider* collider) = 0;

        /** Removes all colliders from the child list of this rigidbody. */
        virtual void RemoveColliders() = 0;

        /**
         * Recalculates rigidbody's mass, inertia tensors and center of mass depending on the currently set child colliders.
         * This should be called whenever relevant child collider properties change (like mass or shape).
         *
         * If automatic tensor calculation is turned off then this will do nothing. If automatic mass calculation is turned
         * off then this will use the mass set directly on the body using setMass().
         */
        virtual void UpdateMassDistribution() { }

        /**
         * Moves the rigidbody to a specific position. This method will ensure physically correct movement, meaning the body
         * will collide with other objects along the way.
         */
        virtual void Move(const Vector3& position) = 0;

        /**
         * Rotates the rigidbody. This method will ensure physically correct rotation, meaning the body will collide with
         * other objects along the way.
         */
        virtual void Rotate(const Quaternion& rotation) = 0;

        /** Returns the current position of the rigidbody. */
        virtual Vector3 GetPosition() const = 0;

        /** Returns the current rotation of the rigidbody. */
        virtual Quaternion GetRotation() const = 0;

        /**
         * Applies new transform values retrieved from the most recent physics update (values resulting from physics
         * simulation).
         */
        virtual void SetTransform(const Vector3& position, const Quaternion& rotation) = 0;

        /** Triggered when one of the colliders owned by the rigidbody starts colliding with another object. */
        Event<void(const CollisionDataRaw&)> OnCollisionBegin;

        /** Triggered when a previously colliding collider stays in collision. Triggered once per frame. */
        Event<void(const CollisionDataRaw&)> OnCollisionStay;

        /** Triggered when one of the colliders owned by the rigidbody stops colliding with another object. */
        Event<void(const CollisionDataRaw&)> OnCollisionEnd;

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

    protected:
        HSceneObject _linkedSO;
        PhysicsObjectOwner _owner;
        FBody* _internal = nullptr;
    };
}
