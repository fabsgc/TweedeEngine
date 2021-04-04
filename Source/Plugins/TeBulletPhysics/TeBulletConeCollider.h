#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeConeCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a ConeCollider. */
    class BulletConeCollider : public ConeCollider
    {
    public:
        BulletConeCollider(BulletPhysics* physics);
        ~BulletConeCollider();
    };
}
