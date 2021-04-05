#include "TeBulletCylinderCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletCylinderCollider::BulletCylinderCollider(BulletPhysics* physics, BulletScene* scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
    }

    BulletCylinderCollider::~BulletCylinderCollider()
    { 
        te_delete(_internal);
    }
}
