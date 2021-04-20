#include "TeBulletFCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletFCollider::BulletFCollider(BulletPhysics* physics, BulletScene* scene, btCollisionShape* shape)
        : _physics(physics)
        , _scene(scene)
        , _shape(shape)
    { 
        SetBtTransform();
    }

    BulletFCollider::~BulletFCollider()
    { }

    void BulletFCollider::SetCenter(const Vector3& center)
    {
        FCollider::SetCenter(center);
        SetBtTransform();
    }

    void BulletFCollider::SetPosition(const Vector3& position)
    {
        FCollider::SetPosition(position);
        SetBtTransform();
    }

    void BulletFCollider::SetRotation(const Quaternion& rotation)
    {
        FCollider::SetRotation(rotation);
        SetBtTransform();
    }

    void BulletFCollider::SetBtTransform()
    {
        _tranform.setOrigin(ToBtVector3(_position + ToQuaternion(_tranform.getRotation()) * _center));
        _tranform.setRotation(ToBtQuaternion(_rotation));
    }

    const btTransform& BulletFCollider::GetBtTransform()
    {
        return _tranform;
    }
}
