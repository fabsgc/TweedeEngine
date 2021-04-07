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

        /** @copydoc Body::Move */
        void Move(const Vector3& position) override;

        /** @copydoc Body::Rotate */
        void Rotate(const Quaternion& rotation) override;

        /** @copydoc Body::GetPosition */
        Vector3 GetPosition() const override;

        /** @copydoc Body::GetRotation */
        Quaternion GetRotation() const override;

        /** @copydoc Body::SetTransform */
        void SetTransform(const Vector3& pos, const Quaternion& rot) override;

        /** @copydoc Body::SetMass */
        void SetMass(float mass) override;

        /** @copydoc Body::GetMass */
        float GetMass() const override;

        /** @copydoc Body::SetIsKinematic */
        void SetIsKinematic(bool kinematic) override;

        /** @copydoc Body::GetIsKinematic */
        bool GetIsKinematic() const override;

        /** @copydoc Body::AddCollider() */
        void AddCollider(Collider* collider) override;

        /** @copydoc Body::RemoveCollider() */
        void RemoveCollider(Collider* collider) override;

        /** @copydoc Body::RemoveColliders() */
        void RemoveColliders() override;

        /** @copydoc Body::SetFlags */
        void SetFlags(BodyFlag flags) override;

    private:
        btRigidBody* _rigidBody;
    };
}
