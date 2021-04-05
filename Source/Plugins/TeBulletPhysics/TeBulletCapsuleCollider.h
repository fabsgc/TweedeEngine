#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeCapsuleCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a CapsuleCollider. */
    class BulletCapsuleCollider : public CapsuleCollider
    {
    public:
        BulletCapsuleCollider(BulletPhysics* physics, BulletScene* scene);
        ~BulletCapsuleCollider();
    };
}
