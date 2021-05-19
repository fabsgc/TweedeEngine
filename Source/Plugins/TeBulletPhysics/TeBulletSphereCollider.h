#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSphereCollider.h"
#include "TeBulletCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a SphereCollider. */
    class BulletSphereCollider : public SphereCollider, public BulletCollider
    {
    public:
        BulletSphereCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
            const Quaternion& rotation, float radius);
        ~BulletSphereCollider();

        /** @copydoc SphereCollider::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc SphereCollider::SetRadius */
        void SetRadius(float radius) override;

        /** @copydoc SphereCollider::GetRadius */
        float GetRadius() const override { return _radius; }

    private:
        /** @copydoc BulletCollider::UpdateCollider */
        void UpdateCollider() override;

    private:
        btSphereShape* _shape = nullptr;
        float _radius;
    };
}
