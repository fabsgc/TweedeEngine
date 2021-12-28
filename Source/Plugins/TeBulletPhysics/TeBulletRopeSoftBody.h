#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeRopeSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{
    /** Bullet implementation of a RopeSoftBody. */
    class BulletRopeSoftBody : public RopeSoftBody, public BulletSoftBody
    {
    public:
        BulletRopeSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletRopeSoftBody();

        /** @copydoc Body::Update */
        void Update() override;

        /** @copydoc RopeSoftBody::SetFrom */
        void SetFrom(const Vector3& from) override;

        /** @copydoc RopeSoftBody::GetFrom */
        Vector3 GetFrom() const override;

        /** @copydoc RopeSoftBody::SetTo */
        void SetTo(const Vector3& to) override;

        /** @copydoc RopeSoftBody::GetTo */
        Vector3 GetTo() const override;

        /** @copydoc RopeSoftBody::SetResolution */
        void SetResolution(UINT32 resolution) override;

        /** @copydoc RopeSoftBody::GetResolution */
        UINT32 GetResolution() const override;

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

    private:
        Vector3 _from = Vector3(-2.0f, 10.0f, -2.0f);
        Vector3 _to = Vector3(0.0f, 2.0f, 0.0f);
        UINT32 _resolution = 8;
    };
}
