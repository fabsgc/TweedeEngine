#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSliderJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a Slider joint. */
    class BulletSliderJoint : public SliderJoint, public BulletJoint
    {
    public:
        BulletSliderJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletSliderJoint();

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
