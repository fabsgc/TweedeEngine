#include "TeBulletFBody.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletFBody::BulletFBody(BulletPhysics* physics, BulletScene* scene)
        : _physics(physics)
        , _scene(scene)
    { }

    BulletFBody::~BulletFBody()
    { }
}
