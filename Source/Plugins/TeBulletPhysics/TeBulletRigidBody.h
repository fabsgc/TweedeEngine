#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Bullet implementation of a RigidBody. */
    class BulletRigidBody : public RigidBody
    {
    public:
        BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletRigidBody();

        /** @copydoc RigidBody::Move */
        void Move(const Vector3& position) override;

        /** @copydoc RigidBody::Rotate */
        void Rotate(const Quaternion& rotation) override;

        /** @copydoc RigidBody::GetPosition */
        Vector3 GetPosition() const override;

        /** @copydoc RigidBody::GetRotation */
        Quaternion GetRotation() const override;

        /** @copydoc RigidBody::SetTransform */
        void SetTransform(const Vector3& pos, const Quaternion& rot) override;

        /** @copydoc RigidBody::AddCollider() */
        void AddCollider(Collider* collider) override;

        /** @copydoc RigidBody::RemoveCollider() */
        void RemoveCollider(Collider* collider) override;

        /** @copydoc RigidBody::RemoveColliders() */
        void RemoveColliders() override;

    private:
        btRigidBody* _rigidBody;
    };
}
