#include "TeBulletPlaneCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletPlaneCollider::BulletPlaneCollider(BulletPhysics* physics, BulletScene* scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
    }

    BulletPlaneCollider::~BulletPlaneCollider()
    { 
        te_delete(_internal);
    }
}
