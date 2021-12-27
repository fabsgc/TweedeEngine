#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePatchSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{
    /** Bullet implementation of a PatchSoftBody. */
    class BulletPatchSoftBody : public PatchSoftBody, public BulletSoftBody
    {
    public:
        BulletPatchSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletPatchSoftBody();

        /** @copydoc Body::Update */
        void Update() override;

        /** @copydoc PatchSoftBody::SetCorners */
        void SetCorners(const Vector3& topLeft, const Vector3& topRight, const Vector3& bottomLeft, const Vector3& bottomRight) override;

        /** @copydoc PatchSoftBody::SetCorners */
        void GetCorners(Vector3& topLeft, Vector3& topRight, Vector3& bottomLeft, Vector3& bottomRight) const override;

        /** @copydoc PatchSoftBody::SetCorners */
        void SetResolution(UINT32 x, UINT32 y) override;

        /** @copydoc PatchSoftBody::SetCorners */
        void GetResolution(UINT32& x, UINT32& y) const override;

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

    private:
        Vector3 _topLeft;
        Vector3 _topRight;
        Vector3 _bottomLeft;
        Vector3 _bottomRight;
        UINT32 _resolutionX;
        UINT32 _resolutionY;
    };
}
