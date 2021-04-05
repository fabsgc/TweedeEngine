#include "TeBulletSphereCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletSphereCollider::BulletSphereCollider(BulletPhysics* physics, BulletScene* scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
    }

    BulletSphereCollider::~BulletSphereCollider()
    { 
        te_delete(_internal);
    }
}
