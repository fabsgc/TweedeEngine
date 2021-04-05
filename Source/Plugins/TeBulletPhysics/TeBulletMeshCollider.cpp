#include "TeBulletMeshCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletMeshCollider::BulletMeshCollider(BulletPhysics* physics, BulletScene* scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
    }

    BulletMeshCollider::~BulletMeshCollider()
    { 
        te_delete(_internal);
    }
}
