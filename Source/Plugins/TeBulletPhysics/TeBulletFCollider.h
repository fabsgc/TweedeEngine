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
        explicit BulletFCollider(BulletPhysics* physics, BulletScene* scene, btCollisionShape* shape = nullptr);
        ~BulletFCollider();

        /** @copydoc FCollider::SetIsTrigger */
        void SetIsTrigger(bool value) override;

        /** @copydoc FCollider::GetIsTrigger */
        bool GetIsTrigger() const override;

        /** @copydoc FCollider::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode) override;

        /** @copydoc SetCollisionReportMode() */
        CollisionReportMode GetCollisionReportMode() const override;

        /** Set current btCollisionShape */
        void SetShape(btCollisionShape* shape) { _shape = shape; }

        /** Return current btCollisionShape */
        const auto& GetShape() const { return _shape; }

    protected:
        BulletPhysics* _physics;
        BulletScene* _scene;

        bool _isTrigger = false;
        bool _isStatic = true;

        btCollisionShape* _shape;

        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
