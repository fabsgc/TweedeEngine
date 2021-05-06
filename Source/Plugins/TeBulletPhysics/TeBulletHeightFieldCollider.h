#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeHeightFieldCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a HeightFieldCollider. */
    class BulletHeightFieldCollider : public HeightFieldCollider
    {
    public:
        BulletHeightFieldCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position, const Quaternion& rotation);
        ~BulletHeightFieldCollider();

        /** @copydoc SphereCollider::SetScale() */
        void SetScale(const Vector3& scale) override;

    private:
        /** Create shape using current parameters */
        void UpdateShape();

        /** @copydoc HeightFieldCollider::OnHeightFieldChanged */
        void OnHeightFieldChanged() override;

    private:
        btHeightfieldTerrainShape* _shape = nullptr;
    };
}
