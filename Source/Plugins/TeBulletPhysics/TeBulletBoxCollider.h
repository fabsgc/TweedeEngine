#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeBoxCollider.h"
#include "TeBulletCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a BoxCollider. */
    class BulletBoxCollider : public BoxCollider, public BulletCollider
    {
    public:
        BulletBoxCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
            const Quaternion& rotation, const Vector3& extents);
        virtual ~BulletBoxCollider();

        /** @copydoc BoxCollider::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc BoxCollider::SetExtents */
        void SetExtents(const Vector3& extents) override;

        /** @copydoc BoxCollider::GetExtents */
        Vector3 GetExtents() const override { return _extents; }

    private:
        /** @copydoc BulletCollider::UpdateCollider */
        void UpdateCollider() override;

    private:
        btBoxShape* _shape = nullptr;
        Vector3 _extents;
    };
}
