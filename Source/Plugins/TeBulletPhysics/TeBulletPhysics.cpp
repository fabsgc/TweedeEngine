#include "TeBulletPhysics.h"
#include "TeCoreApplication.h"

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

namespace te
{
    BulletPhysics::BulletPhysics(const PHYSICS_INIT_DESC& input)
        : Physics(input)
        , _initDesc(input)
        , _paused(false)
    { 
        new btSoftBodyRigidBodyCollisionConfiguration();
    }

    BulletPhysics::~BulletPhysics()
    { }

    void BulletPhysics::SetPaused(bool paused)
    {
        _paused = paused;
    }

    bool BulletPhysics::IsPaused() const
    {
        return _paused;
    }

    void BulletPhysics::Update()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Physics);
        if (IsPaused() || !isRunning)
            return;
    }
}
