#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Utility/TeEvent.h"

#include <cfloat>

namespace te
{
    class Ray;
    class PhysicsScene;

    /** Type of force or torque that can be applied to a body. */
    enum class ForceMode
    {
        Force, /**< Value applied is a force. */
        Impulse, /**< Value applied is an impulse (a direct change in its linear or angular momentum). */
    };

    /** Flags that control options of a body object. */
    enum class BodyFlag
    {
        /** No options. */
        None = 0x00,
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

        /** Allows to be able to update body state */
        virtual void Update() = 0;

        /** Returns the current position of the body. */
        virtual Vector3 GetPosition() const = 0;

        /** Returns the current rotation of the body. */
        virtual Quaternion GetRotation() const = 0;

        /**
         * Applies new transform values retrieved from the most recent physics update (values resulting from physics
         * simulation).
         */
        virtual void SetTransform(const Vector3& position, const Quaternion& rotation, bool activate = false) = 0;

        /**
         * Enables/disables a body as a trigger. A trigger will not be used for collisions (objects will pass
         * through it), but collision events will still be reported.
         */
        virtual void SetIsTrigger(bool trigger) = 0;

        /** @copydoc GetIsTrigger */
        virtual bool GetIsTrigger() const = 0;

        /** For debugging purpose only, specify if this body will be displayed if physic debug information are enabled */
        virtual void SetIsDebug(bool debug) = 0;

        /** @copydoc SetIsDebug */
        virtual bool GetIsDebug() const = 0;

        /**
         * Determines the mass of the object and all of its collider shapes. Only relevant if BodyFlag::AutoMass or
         * BodyFlag::AutoTensors is turned off. Value of zero means the object is immovable (but can be rotated).
         */
        virtual void SetMass(float mass) = 0;

        /** @copydoc setMass */
        virtual float GetMass() const = 0;

        /** Determines the linear velocity of the body. */
        virtual void SetVelocity(const Vector3& velocity) = 0;

        /** @copydoc SetVelocity */
        virtual const Vector3& GetVelocity() const = 0;

        /** Determines the angular velocity of the body. */
        virtual void SetAngularVelocity(const Vector3& velocity) = 0;

        /** @copydoc SetAngularVelocity */
        virtual const Vector3& GetAngularVelocity() const = 0;

        /**
         * Determines if the body is kinematic. Kinematic body will not move in response to external forces (for example
         * gravity, or another object pushing it), essentially behaving like collider. Unlike a collider though, you can
         * still move the object and have other dynamic objects respond correctly (meaning it will push other objects).
         */
        virtual void SetIsKinematic(bool kinematic) = 0;

        /** @copydoc SetIsKinematic */
        virtual bool GetIsKinematic() const = 0;

        /** Determines the linear drag of the body. Higher drag values means the object resists linear movement more. */
        virtual void SetFriction(float friction) = 0;

        /** @copydoc SetFriction */
        virtual float GetFriction() const = 0;

        /** Determines the angular drag of the body. Higher drag values means the object resists angular movement more. */
        virtual void SetRollingFriction(float rollingFriction) = 0;

        /** @copydoc GeRollingtFriction */
        virtual float GetRollingFriction() const = 0;

        /** Determines the restitution of the body. */
        virtual void SetRestitution(float restitution) = 0;

        /** @copydoc GetRestitution */
        virtual float GetRestitution() const = 0;

        /** Determines whether or not the body will have the global gravity force applied to it. */
        virtual void SetUseGravity(bool gravity) = 0;

        /** @copydoc SetUseGravity */
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

        /** Sets a value that determines which (if any) collision events are reported. */
        virtual void SetCollisionReportMode(CollisionReportMode mode) = 0;

        /** Gets a value that determines which (if any) collision events are reported. */
        virtual CollisionReportMode GetCollisionReportMode() const { return _collisionReportMode; }
        
        /** Flags that control the behaviour of the body. */
        virtual void SetFlags(BodyFlag flags) { _flags = flags; }

        /** @copydoc SetFlags */
        virtual BodyFlag GetFlags() const { return _flags; }

        /** Registers a new collider as a child of this body. */
        virtual void AddCollider(Collider*) = 0;

        /** Removes a collider from the child list of this body. */
        virtual void RemoveCollider(Collider*) = 0;

        /** Removes all colliders from the child list of this body. */
        virtual void RemoveColliders() = 0;

        /** Register a new joint as a child of this body */
        virtual void AddJoint(Joint* joint) = 0;

        /** Removes a joint from the child list of this body. */
        virtual void RemoveJoint(Joint* joint) = 0;

        /** Removes all joints from the child list of this body. */
        virtual void RemoveJoints() = 0;

        /**
         * Checks does the ray hit this body.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	ray		        Ray to check.
         * @param[out]	hit		        Information about the hit. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the body.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, PhysicsQueryHit& hit, float maxDist = FLT_MAX) const;

        /**
         * Checks does the ray hit this body.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	origin	        Origin of the ray to check.
         * @param[in]	unitDir	        Unit direction of the ray to check.
         * @param[out]	hit		        Information about the hit. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the body.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit,
            float maxDist = FLT_MAX) const;

        /**
         * Checks does the ray hit this body.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	ray		        Ray to check.
         * @param[out]	hit		        Information about all the hits. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the body.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, Vector<PhysicsQueryHit>& hit, float maxDist = FLT_MAX) const;

        /**
         * Checks does the ray hit this body.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	origin	        Origin of the ray to check.
         * @param[in]	unitDir	        Unit direction of the ray to check.
         * @param[out]	hit		        Information about all the hits. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the body.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, Vector<PhysicsQueryHit>& hit,
            float maxDist = FLT_MAX) const;

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

        /** In case this body is moved within the scene, we should need to update its _linkedSO */
        void _setLinkedSO(const HSceneObject& SO);

        /** Returns the object containing common collider code. */
        FBody* GetInternal() const { return _internal; }

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
        FBody* _internal = nullptr;
        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::None);
        HSceneObject _linkedSO;
        PhysicsObjectOwner _owner;
        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
