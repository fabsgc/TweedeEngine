#include "TeBulletFCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletFCollider::BulletFCollider(BulletPhysics* physics, BulletScene* scene)
        : _physics(physics)
        , _scene(scene)
    { }

    BulletFCollider::~BulletFCollider()
    { }

    void BulletFCollider::SetIsTrigger(bool value)
    {
        if(value)
        {
            _isTrigger = true;
        }
        else
        {
            _isTrigger = false;
        }
    }

    bool BulletFCollider::GetIsTrigger() const
    {
        return _isTrigger;
    }

    void BulletFCollider::SetIsStatic(bool value)
    {
        if (_isStatic == value)
            return;

        _isStatic = value;
    }

    bool BulletFCollider::GetIsStatic() const
    {
        return _isStatic;
    }
}
