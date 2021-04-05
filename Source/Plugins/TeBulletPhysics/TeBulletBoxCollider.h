#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeBoxCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a BoxCollider. */
    class BulletBoxCollider : public BoxCollider
    {
    public:
        BulletBoxCollider(BulletPhysics* physics, BulletScene* scene);
        ~BulletBoxCollider();
    };
}
