#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSphereCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a SphereCollider. */
    class BulletSphereCollider : public SphereCollider
    {
    public:
        BulletSphereCollider(BulletPhysics* physics);
        ~BulletSphereCollider();
    };
}
