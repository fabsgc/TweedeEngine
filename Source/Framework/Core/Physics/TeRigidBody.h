#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"

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
    class TE_CORE_EXPORT RigidBody
    {
    public:
        /**
         * Constructs a new rigidbody.
         *
         * @param[in]	linkedSO	Scene object that owns this rigidbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        RigidBody(const HSceneObject& linkedSO);
        virtual ~RigidBody() = default;

        /**
         * Creates a new rigidbody.
         *
         * @param[in]	linkedSO	Scene object that owns this rigidbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        static SPtr<RigidBody> Create(const HSceneObject& linkedSO);

    protected:
        HSceneObject _linkedSO;
    };
}
