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

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

        /** @copydoc BulletSoftBody::Release */
        void Release() override;

        /** @copydoc BulletSoftBody::RemoveFromWorld */
        void RemoveFromWorld() override;

        /** @copydoc BulletSoftBody::UpdateKinematicFlag */
        void UpdateKinematicFlag() const override;

        /** @copydoc BulletSoftBody::UpdateCCDFlag */
        void UpdateCCDFlag() const override;

        /** @copydoc BulletSoftBody::Activate */
        void Activate() const override;

        /** @copydoc BulletSoftBody::IsActivated */
        bool IsActivated() const override;

    private:
        btSoftBody* _softBody;
        BulletPhysics* _physics;
        BulletScene* _scene;

        bool _isDirty = true;
        bool _inWorld = false;
    };
}
