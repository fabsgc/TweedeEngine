#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSphericalJoint.h"

namespace te
{
    /** Bullet implementation of a Spherical joint. */
    class BulletSphericalJoint : public SphericalJoint
    {
    public:
        BulletSphericalJoint(BulletPhysics* physics, BulletScene* scene, const SPHERICAL_JOINT_DESC& desc);
        ~BulletSphericalJoint();

    private:
        /** Build internal bullet representation of a joint */
        void BuildJoint();

        /** Update the internal representation of a joint */
        void UpdateJoint();

        /** Release the internal representation of a joint from the world */
        void ReleaseJoint();

    private:
        btTypedConstraint* _joint;
    };
}
