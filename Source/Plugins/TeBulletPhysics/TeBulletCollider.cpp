#include "TeBulletCollider.h"

namespace te
{
    BulletCollider::BulletCollider(BulletPhysics* physics, BulletScene* scene)
        : _physics(physics)
        , _scene(scene)
    { }
}
