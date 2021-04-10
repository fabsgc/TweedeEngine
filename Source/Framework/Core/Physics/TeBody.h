#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Utility/TeEvent.h"

namespace te
{
    /** Type of force or torque that can be applied to a body. */
    enum class ForceMode
    {
        Force, /**< Value applied is a force. */
        Impulse, /**< Value applied is an impulse (a direct change in its linear or angular momentum). */
        Velocity, /**< Value applied is velocity. */
        Acceleration /**< Value applied is accelearation. */
    };

    /** Type of force that can be applied to a body at an arbitrary point. */
    enum class PointForceMode
    {
        Force, /**< Value applied is a force. */
        Impulse, /**< Value applied is an impulse (a direct change in its linear or angular momentum). */
    };

    /** Flags that control options of a body object. */
    enum class BodyFlag
    {
        /** No options. */
        None = 0x00,
        /** Automatically calculate center of mass transform and inertia tensors from child shapes (colliders). */
        AutoTensors = 0x01,
        /** Calculate mass distribution from child shapes (colliders). Only relevant when auto-tensors is on. */
        AutoMass = 0x02,
        /**
         * Enables continous collision detection. This can prevent fast moving bodies from tunneling through each other.
         * This must also be enabled globally in Physics otherwise the flag will be ignored.
         */
        CCD = 0x04
    };

    /**
     * Base class for all Body types such as RigidBody and SoftBody
     */
    class TE_CORE_EXPORT Body
    {
    public:
        Body(const HSceneObject& linkedSO);
        virtual ~Body() = default;

        /**
         * Moves the body to a specific position. This method will ensure physically correct movement, meaning the body
         * will collide with other objects along the way.
         */
        virtual void Move(const Vector3& position) = 0;

        /**
         * Rotates the body. This method will ensure physically correct rotation, meaning the body will collide with
         * other objects along the way.
         */
        virtual void Rotate(const Quaternion& rotation) = 0;

        /** Returns the current position of the body. */
        virtual Vector3 GetPosition() const = 0;

        /** Returns the current rotation of the body. */
        virtual Quaternion GetRotation() const = 0;

        /**
         * Recalculates body's mass, inertia tensors and center of mass depending on the currently set child colliders.
         * This should be called whenever relevant child collider properties change (like mass or shape).
         *
         * If automatic tensor calculation is turned off then this will do nothing. If automatic mass calculation is turned
         * off then this will use the mass set directly on the body using setMass().
         */
        virtual void UpdateMassDistribution() { }

        /**
         * Applies new transform values retrieved from the most recent physics update (values resulting from physics
         * simulation).
         */
        virtual void SetTransform(const Vector3& position, const Quaternion& rotation, bool activate = false) = 0;

        /**
         * Determines the mass of the object and all of its collider shapes. Only relevant if BodyFlag::AutoMass or
         * BodyFlag::AutoTensors is turned off. Value of zero means the object is immovable (but can be rotated).
         */
        virtual void SetMass(float mass) = 0;

        /** @copydoc setMass() */
        virtual float GetMass() const = 0;

        /** Determines the linear velocity of the body. */
        virtual void SetVelocity(const Vector3& velocity) = 0;

        /** @copydoc SetVelocity() */
        virtual const Vector3& GetVelocity() const = 0;

        /** Determines the angular velocity of the body. */
        virtual void SetAngularVelocity(const Vector3& velocity) = 0;

        /** @copydoc SetAngularVelocity() */
        virtual const Vector3& GetAngularVelocity() const = 0;

        /**
         * Determines if the body is kinematic. Kinematic body will not move in response to external forces (for example
         * gravity, or another object pushing it), essentially behaving like collider. Unlike a collider though, you can
         * still move the object and have other dynamic objects respond correctly (meaning it will push other objects).
         */
        virtual void SetIsKinematic(bool kinematic) = 0;

        /** @copydoc SetIsKinematic() */
        virtual bool GetIsKinematic() const = 0;

        /** Determines the linear drag of the body. Higher drag values means the object resists linear movement more. */
        virtual void SetFriction(float friction) = 0;

        /** @copydoc SetFriction() */
        virtual float GetFriction() const = 0;

        /** Determines the angular drag of the body. Higher drag values means the object resists angular movement more. */
        virtual void SetRollingFriction(float rollingFriction) = 0;

        /** @copydoc GeRollingtFriction() */
        virtual float GetRollingFriction() const = 0;

        /** Determines the restitution of the body. */
        virtual void SetRestitution(float restitution) = 0;

        /** @copydoc GetRestitution() */
        virtual float GetRestitution() const = 0;

        /** Determines whether or not the body will have the global gravity force applied to it. */
        virtual void SetUseGravity(bool gravity) = 0;

        /** @copydoc SetUseGravity() */
        virtual bool GetUseGravity() const = 0;

        /** Sets the body's center of mass transform. Only relevant if RigibodyFlag::AutoTensors is turned off. */
        virtual void SetCenterOfMass(const Vector3& centerOfMass) = 0;

        /** Returns the position of the center of mass. */
        virtual const Vector3& GetCenterOfMass() const = 0;

        /**
         * Applies a force to the center of the mass of the body. This will produce linear momentum.
         *
         * @param[in]	force		Force to apply.
         * @param[in]	mode		Determines what is the type of @p force.
         */
        virtual void ApplyForce(const Vector3& force, ForceMode mode) const = 0;

        /**
         * Applies a force to a specific point on the body. This will in most cases produce both linear and angular
         * momentum.
         *
         * @param[in]	force		Force to apply.
         * @param[in]	position	World position to apply the force at.
         * @param[in]	mode		Determines what is the type of @p force.
         */
        virtual void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const = 0;

        /**
         * Applies a torque to the body. This will produce angular momentum.
         *
         * @param[in]	torque		Torque to apply.
         * @param[in]	mode		Determines what is the type of @p torque.
         */
        virtual void ApplyTorque(const Vector3& torque, ForceMode mode) const = 0;

        /**
         * Recalculates body's mass, inertia tensors and center of mass depending on the currently set child colliders.
         * This should be called whenever relevant child collider properties change (like mass or shape).
         *
         * If automatic tensor calculation is turned off then this will do nothing. If automatic mass calculation is turned
         * off then this will use the mass set directly on the body using setMass().
         */
        virtual void updateMassDistribution() { }
        
        /** Flags that control the behaviour of the body. */
        virtual void SetFlags(BodyFlag flags) { _flags = flags; }

        /** @copydoc SetFlags() */
        virtual BodyFlag GetFlags() const { return _flags; }

        /** Registers a new collider as a child of this body. */
        virtual void AddCollider(const HCollider& collider) = 0;

        /** Removes a collider from the child list of this body. */
        virtual void RemoveCollider(const HCollider& collider) = 0;

        /** Removes all colliders from the child list of this body. */
        virtual void RemoveColliders() = 0;

        /** Triggered when one of the colliders owned by the body starts colliding with another object. */
        Event<void(const CollisionDataRaw&)> OnCollisionBegin;

        /** Triggered when a previously colliding collider stays in collision. Triggered once per frame. */
        Event<void(const CollisionDataRaw&)> OnCollisionStay;

        /** Triggered when one of the colliders owned by the body stops colliding with another object. */
        Event<void(const CollisionDataRaw&)> OnCollisionEnd;

        /**
         * Applies new transform values retrieved from the most recent physics update (values resulting from physics
         * simulation).
         */
        void _setTransform(const Vector3& position, const Quaternion& rotation);

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
        BodyFlag _flags;
        HSceneObject _linkedSO;
        PhysicsObjectOwner _owner;
        FBody* _internal = nullptr;
    };
}
