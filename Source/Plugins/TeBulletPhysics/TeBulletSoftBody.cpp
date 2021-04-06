#include "TeBulletSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFBody.h"

namespace te
{
    BulletSoftBody::BulletSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : SoftBody(linkedSO)
    { 
        _internal = te_new<BulletFBody>(physics, scene);
    }

    BulletSoftBody::~BulletSoftBody()
    { 
        te_delete(_internal);  // TODO
    }

    void BulletSoftBody::Move(const Vector3& position)
    {
        // TODO
    }

    void BulletSoftBody::Rotate(const Quaternion& rotation)
    {
        // TODO
    }

    Vector3 BulletSoftBody::GetPosition() const
    {
        return Vector3::ZERO; // TODO
    }

    Quaternion BulletSoftBody::GetRotation() const
    {
        return Quaternion::IDENTITY; // TODO
    }

    void BulletSoftBody::SetTransform(const Vector3& pos, const Quaternion& rot)
    {
        // TODO
    }

    void BulletSoftBody::AddCollider(Collider* collider)
    {
        TE_PRINT("Add Collider");
    }

    void BulletSoftBody::RemoveCollider(Collider* collider)
    {
        TE_PRINT("Remove Collider");
    }

    void BulletSoftBody::RemoveColliders()
    {
        TE_PRINT("Remove all Colliders");
    }
}
