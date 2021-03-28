#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysics.h"
#include "Physics/TePhysicsCommon.h"

namespace te 
{
    /** Bullet implementation of Physics. */
    class BulletPhysics : public Physics
    {
    public:
        BulletPhysics(const PHYSICS_INIT_DESC& input);
        ~BulletPhysics();

        /** @copydoc Physics::SetPaused */
        void SetPaused(bool paused) override;

        /** @copydoc Physics::SetPaused */
        bool IsPaused() const override;

        /** @copydoc Physics::Update */
        void Update() override;

    private:
        PHYSICS_INIT_DESC _initDesc;
        bool _paused;

        btBroadphaseInterface* _broadphase = nullptr;
        btCollisionDispatcher* _collisionDispatcher = nullptr;
        btSequentialImpulseConstraintSolver* _constraintSolver = nullptr;
        btDefaultCollisionConfiguration* _collisionConfiguration = nullptr;
        btDiscreteDynamicsWorld* _world = nullptr;
        btSoftBodyWorldInfo* _worldInfo = nullptr;

        UINT32 _maxSubSteps = 1;
        UINT32 _maxSolveIterations = 256;
        float _internalFps = 60.0f;
    };
}
