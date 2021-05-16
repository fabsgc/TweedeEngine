#pragma once

#include "TeBulletPhysicsPrerequisites.h"

namespace te
{
    /** Bullet implementation of joint. */
    class BulletJoint
    {
    public:
        BulletJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletJoint();

    protected:
        friend class BulletFJoint;

        /** Build internal bullet representation of a joint */
        virtual void BuildJoint() = 0;

        /** Update the internal representation of a joint */
        virtual void UpdateJoint() = 0;

        /** Release the internal representation of a joint from the world */
        virtual void ReleaseJoint() = 0;

    protected:
        BulletPhysics* _physics;
        BulletScene* _scene;
    };
}
