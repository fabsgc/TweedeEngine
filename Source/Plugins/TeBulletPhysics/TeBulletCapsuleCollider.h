#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeCapsuleCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a CapsuleCollider. */
    class BulletCapsuleCollider : public CapsuleCollider
    {
    public:
        BulletCapsuleCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
            const Quaternion& rotation, float radius, float height);
        ~BulletCapsuleCollider();

        /** @copydoc CapsuleCollider::SetScale() */
        void SetScale(const Vector3& scale) override;

        /** @copydoc CapsuleCollider::SetRadius() */
        void SetRadius(float radius) override;

        /** @copydoc CapsuleCollider::GetRadius() */
        float GetRadius() const override { return _radius; }

        /** @copydoc CapsuleCollider::SetRadius() */
        void SetHeight(float height) override;

        /** @copydoc CapsuleCollider::GetRadius() */
        float GetHeight() const override { return _height; }

    private:
        /** Create shape using current parameters */
        void UpdateShape();

    private:
        btCapsuleShape* _shape = nullptr;
        float _radius;
        float _height;
    };
}
