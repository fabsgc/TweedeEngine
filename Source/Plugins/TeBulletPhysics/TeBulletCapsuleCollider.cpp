#include "TeBulletCapsuleCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletCapsuleCollider::BulletCapsuleCollider(BulletPhysics* physics, BulletScene* scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
    }

    BulletCapsuleCollider::~BulletCapsuleCollider()
    { 
        te_delete(_internal);
    }
}
