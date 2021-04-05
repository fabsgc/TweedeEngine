#include "TeBulletConeCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletConeCollider::BulletConeCollider(BulletPhysics* physics, BulletScene* scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
    }

    BulletConeCollider::~BulletConeCollider()
    { 
        te_delete(_internal);
    }
}
