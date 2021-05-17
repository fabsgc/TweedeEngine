#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeConeCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a ConeCollider. */
    class BulletConeCollider : public ConeCollider
    {
    public:
        BulletConeCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
            const Quaternion& rotation, float radius, float height);
        ~BulletConeCollider();

        /** @copydoc ConeCollider::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc ConeCollider::SetRadius */
        void SetRadius(float radius) override;

        /** @copydoc ConeCollider::GetRadius */
        float GetRadius() const override { return _radius; }

        /** @copydoc ConeCollider::SetRadius */
        void SetHeight(float height) override;

        /** @copydoc ConeCollider::GetRadius */
        float GetHeight() const override { return _height; }

    private:
        /** Create shape using current parameters */
        void UpdateShape();

    private:
        btConeShape* _shape = nullptr;
        float _radius;
        float _height;
    };
}
