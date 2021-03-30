#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSphericalJoint.h"

namespace te
{
    /** Bullet implementation of a Spherical joint. */
    class BulletSphericalJoint : public SphericalJoint
    {
    public:
        BulletSphericalJoint(BulletPhysics* physics, const SPHERICAL_JOINT_DESC& desc);
        ~BulletSphericalJoint();
    };
}
