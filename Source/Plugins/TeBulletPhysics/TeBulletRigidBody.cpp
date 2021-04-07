#include "TeBulletRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFBody.h"

namespace te
{
    BulletRigidBody::BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : RigidBody(linkedSO)
        , _rigidBody(nullptr)
    { 
        _internal = te_new<BulletFBody>(physics, scene);
    }

    BulletRigidBody::~BulletRigidBody()
    { 
        te_delete(_internal);  // TODO
    }

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

    void BulletRigidBody::SetMass(float mass)
    {
        // TODO
    }

    float BulletRigidBody::GetMass() const
    {
        return 0.0f; // TODO
    }

    void BulletRigidBody::SetIsKinematic(bool kinematic)
    {
        // TODO
    }

    bool BulletRigidBody::GetIsKinematic() const
    {
        return false; // TODO
    }

    void BulletRigidBody::AddCollider(Collider* collider)
    {
        TE_PRINT("Add Collider");
    }

    void BulletRigidBody::RemoveCollider(Collider* collider)
    {
        TE_PRINT("Remove Collider");
    }

    void BulletRigidBody::RemoveColliders()
    {
        TE_PRINT("Remove all Colliders");
    }

    void BulletRigidBody::SetFlags(BodyFlag flags)
    {
        // TODO
    }
}
