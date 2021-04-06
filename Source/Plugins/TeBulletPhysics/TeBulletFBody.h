#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeFBody.h"

namespace te
{
    /** Bullet implementation of FBody. */
    class BulletFBody : public FBody
    {
    public:
        explicit BulletFBody(BulletPhysics* physics, BulletScene* scene);
        ~BulletFBody();

    protected:
        BulletPhysics* _physics;
        BulletScene* _scene;
    };
}
