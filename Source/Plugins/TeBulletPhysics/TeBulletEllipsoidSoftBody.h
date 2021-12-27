#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeEllipsoidSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{
    /** Bullet implementation of a EllipsoidSoftBody. */
    class BulletEllipsoidSoftBody : public EllipsoidSoftBody, public BulletSoftBody
    {
    public:
        BulletEllipsoidSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletEllipsoidSoftBody();

        /** @copydoc Body::Update */
        void Update() override;

        /** @copydoc EllipsoidSoftBody::SetCenter */
        void SetCenter(const Vector3& center) override;

        /** @copydoc EllipsoidSoftBody::GetCenter */
        Vector3 GetCenter() const override;

        /** @copydoc EllipsoidSoftBody::SetRadius */
        void SetRadius(const Vector3& radius) override;

        /** @copydoc EllipsoidSoftBody::GetRadius */
        Vector3 GetRadius() const override;

        /** @copydoc EllipsoidSoftBody::SetResolution */
        void SetResolution(UINT32 resolution) override;

        /** @copydoc EllipsoidSoftBody::GetResolution */
        UINT32 GetResolution() const override;

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

    private:
        Vector3 _center = Vector3::ZERO;
        Vector3 _radius = Vector3::ONE;
        UINT32 _resolution = 16;
    };
}
