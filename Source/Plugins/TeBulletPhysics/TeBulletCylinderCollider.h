#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeCylinderCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a CylinderCollider. */
    class BulletCylinderCollider : public CylinderCollider
    {
    public:
        BulletCylinderCollider(BulletPhysics* physics);
        ~BulletCylinderCollider();
    };
}
