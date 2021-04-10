#include "TeBulletFCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletFCollider::BulletFCollider(BulletPhysics* physics, BulletScene* scene, btCollisionShape* shape)
        : _physics(physics)
        , _scene(scene)
        , _shape(shape)
    { }

    BulletFCollider::~BulletFCollider()
    { }

    void BulletFCollider::SetIsTrigger(bool value)
    {
        if(value)
            _isTrigger = true;
        else
            _isTrigger = false;
    }

    bool BulletFCollider::GetIsTrigger() const
    {
        return _isTrigger;
    }

    CollisionReportMode BulletFCollider::GetCollisionReportMode() const
    {
        return _collisionReportMode;
    }

    void BulletFCollider::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;
        // UpdateFilter(); TODO
    }
}
