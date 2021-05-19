#pragma once

#include "TeBulletPhysicsPrerequisites.h"

namespace te
{
    /** 
     * Bullet implementation of collider. It's a interface to store common 
     * methods and attributes among all colliders managed by bullet 
     */
    class BulletCollider
    {
    public:
        BulletCollider(BulletPhysics* physics, BulletScene* scene);
        ~BulletCollider() = default;

    protected:
        /** Update the internal representation of a collider */
        virtual void UpdateCollider() = 0;

    protected:
        BulletPhysics* _physics;
        BulletScene* _scene;
    };
}
