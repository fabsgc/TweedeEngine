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

    Vector3 BulletFCollider::GetPosition() const
    {
        return Vector3::ZERO; // TODO
    }

    Quaternion BulletFCollider::GetRotation() const
    {
        return Quaternion::IDENTITY; // TODO
    }

    void BulletFCollider::SetTransform(const Vector3& pos, const Quaternion& rotation)
    {
        // TODO
    }

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
