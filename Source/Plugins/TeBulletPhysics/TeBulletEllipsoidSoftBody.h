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
