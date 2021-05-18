#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeHingeJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a Hinge joint. */
    class BulletHingeJoint : public HingeJoint, public BulletJoint
    {
    public:
        BulletHingeJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletHingeJoint();

        /** @copydoc Joint::Update */
        void Update() override { }

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
