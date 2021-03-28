#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSoftBody.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Bullet implementation of a Rigidbody. */
    class BulletSoftBody : public SoftBody
    {
    public:
        BulletSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletSoftBody();
    };
}
