#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeBody.h"

namespace te
{
    /**
     * Rigidbody is a dynamic physics object that can be moved using forces (or directly). It will interact with other
     * static and dynamic physics objects in the scene accordingly (it will push other non-kinematic rigidbodies,
     * and collide with static objects).
     *
     * The shape and mass of a rigidbody is governed by its colliders. You must attach at least one collider for the
     * rigidbody to be valid.
     */
    class TE_CORE_EXPORT RigidBody : public Body
    {
    public:

        /**
         * Creates a new rigidbody.
         *
         * @param[in]	linkedSO	Scene object that owns this rigidbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        static SPtr<RigidBody> Create(const HSceneObject& linkedSO);

        /** Used to specify how rigid body react to rotation on each axis */
        virtual void SetAngularFactor(const Vector3& angularFactor) = 0;

        /** @copydoc SetAngularFactor */
        virtual const Vector3& GetAngularFactor() const = 0;

        /** Determines whether or not the body will have the global gravity force applied to it. */
        virtual void SetUseGravity(bool gravity) = 0;

        /** @copydoc SetUseGravity */
        virtual bool GetUseGravity() const = 0;

        /** Sets the body's center of mass transform. */
        virtual void SetCenterOfMass(const Vector3& centerOfMass) = 0;

        /** Returns the position of the center of mass. */
        virtual const Vector3& GetCenterOfMass() const = 0;

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

    protected:
        /**
         * Constructs a new rigidbody.
         *
         * @param[in]	linkedSO	Scene object that owns this rigidbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        explicit RigidBody(const HSceneObject& linkedSO);
    };
}
