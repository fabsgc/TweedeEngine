#include "TeBulletRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletRigidBody::BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : RigidBody(linkedSO)
        , _rigidBody(nullptr)
    { }

    BulletRigidBody::~BulletRigidBody() // TODO
    { }

    void BulletRigidBody::Move(const Vector3& position)
    {
        // TODO
    }

    void BulletRigidBody::Rotate(const Quaternion& rotation)
    {
        // TODO
    }

    Vector3 BulletRigidBody::GetPosition() const
    {
        return Vector3::ZERO; // TODO
    }

    Quaternion BulletRigidBody::GetRotation() const
    {
        return Quaternion::IDENTITY; // TODO
    }

    void BulletRigidBody::SetTransform(const Vector3& pos, const Quaternion& rot)
    {
        // TODO
    }

    void BulletRigidBody::AddCollider(Collider* collider)
    {
        // TODO
    }

    void BulletRigidBody::RemoveCollider(Collider* collider)
    {
        // TODO
    }

    void BulletRigidBody::RemoveColliders()
    {
        // TODO
    }
}
