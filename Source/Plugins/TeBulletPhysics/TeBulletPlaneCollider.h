#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePlaneCollider.h"
#include "TeBulletCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a PlaneCollider. */
    class BulletPlaneCollider : public PlaneCollider, public BulletCollider
    {
    public:
        BulletPlaneCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
            const Quaternion& rotation, const Vector3 normal);
        ~BulletPlaneCollider();

        /** @copydoc PlaneCollider::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc PlaneCollider::SetNormal */
        void SetNormal(const Vector3& normal) override;

        /** @copydoc PlaneCollider::GetNormal */
        const Vector3& GetNormal() const override { return _normal; }

    private:
        /** @copydoc BulletCollider::UpdateCollider */
        void UpdateCollider() override;

    private:
        btStaticPlaneShape* _shape = nullptr;
        Vector3 _normal;
    };
}
