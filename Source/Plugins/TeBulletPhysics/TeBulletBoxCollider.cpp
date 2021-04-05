#include "TeBulletBoxCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletBoxCollider::BulletBoxCollider(BulletPhysics* physics, BulletScene* scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
    }

    BulletBoxCollider::~BulletBoxCollider()
    { 
        te_delete(_internal);
    }
}
