#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePlaneCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a PlaneCollider. */
    class BulletPlaneCollider : public PlaneCollider
    {
    public:
        BulletPlaneCollider(BulletPhysics* physics, BulletScene* scene);
        ~BulletPlaneCollider();
    };
}
