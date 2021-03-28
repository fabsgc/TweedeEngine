#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Bullet implementation of a Rigidbody. */
    class BulletRigidBody : public RigidBody
    {
    public:
        BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletRigidBody();
    };
}
