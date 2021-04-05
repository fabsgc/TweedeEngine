#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeFCollider.h"

namespace te
{
    /** Bullet implementation of FCollider. */
    class BulletFCollider : public FCollider
    {
    public:
        explicit BulletFCollider(BulletPhysics* physics, BulletScene* scene);
        ~BulletFCollider();

        /** @copydoc FCollider::GetPosition */
        Vector3 GetPosition() const override;

        /** @copydoc FCollider::GetRotation */
        Quaternion GetRotation() const override;

        /** @copydoc FCollider::SetTransform */
        void SetTransform(const Vector3& pos, const Quaternion& rotation) override;

        /** @copydoc FCollider::SetIsTrigger */
        void SetIsTrigger(bool value) override;

        /** @copydoc FCollider::GetIsTrigger */
        bool GetIsTrigger() const override;

        /** @copydoc FCollider::SetIsStatic */
        void SetIsStatic(bool value) override;

        /** @copydoc FCollider::GetIsStatic */
        bool GetIsStatic() const override;

        /** @copydoc FCollider::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode) override;

        /** @copydoc SetCollisionReportMode() */
        CollisionReportMode GetCollisionReportMode() const override;

    protected:
        BulletPhysics* _physics;
        BulletScene* _scene;

        bool _isTrigger = false;
        bool _isStatic = true;

        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
