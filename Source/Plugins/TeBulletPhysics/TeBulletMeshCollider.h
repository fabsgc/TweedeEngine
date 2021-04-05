#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeMeshCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a MeshCollider. */
    class BulletMeshCollider : public MeshCollider
    {
    public:
        BulletMeshCollider(BulletPhysics* physics, BulletScene* scene);
        ~BulletMeshCollider();
    };
}
