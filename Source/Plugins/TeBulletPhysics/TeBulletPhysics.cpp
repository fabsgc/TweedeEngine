#include "TeBulletPhysics.h"
#include "TeCoreApplication.h"

namespace te
{
    BulletPhysics::BulletPhysics(const PHYSICS_INIT_DESC& desc)
        : Physics(desc)
        , _initDesc(desc)
        , _paused(false)
    { 
        _broadphase = te_new<btDbvtBroadphase>();
        _constraintSolver = te_new<btSequentialImpulseConstraintSolver>();

        if (_initDesc.SoftBody)
        {
            // Create
            _collisionConfiguration = te_new<btSoftBodyRigidBodyCollisionConfiguration>();
            _collisionDispatcher = te_new<btCollisionDispatcher>(_collisionConfiguration);
            _world = te_new<btSoftRigidDynamicsWorld>(_collisionDispatcher, _broadphase,
                _constraintSolver, _collisionConfiguration);

            // Setup         
            _worldInfo = te_new<btSoftBodyWorldInfo>();
            _worldInfo->m_sparsesdf.Initialize();

            _world->getDispatchInfo().m_enableSPU = true;
            _worldInfo->m_dispatcher = _collisionDispatcher;
            _worldInfo->m_broadphase = _broadphase;
            _worldInfo->air_density = (btScalar)_initDesc.AirDensity;
            _worldInfo->water_density = 0;
            _worldInfo->water_offset = 0;
            _worldInfo->water_normal = btVector3(0, 0, 0);
            _worldInfo->m_gravity = ToBtVector3(_initDesc.Gravity);
        }
        else
        {
            // Create
            _collisionConfiguration = te_new<btDefaultCollisionConfiguration>();
            _collisionDispatcher = te_new<btCollisionDispatcher>(_collisionConfiguration);
            _world = te_new<btDiscreteDynamicsWorld>(_collisionDispatcher, _broadphase,
                _constraintSolver, _collisionConfiguration);
        }

        // Setup
        _world->setGravity(ToBtVector3(_initDesc.Gravity));
        _world->getDispatchInfo().m_useContinuous = true;
        _world->getSolverInfo().m_splitImpulse = false;
        _world->getSolverInfo().m_numIterations = _maxSolveIterations;
    }

    BulletPhysics::~BulletPhysics()
    { 
        te_delete(_world);
        te_delete(_constraintSolver);
        te_delete(_collisionDispatcher);
        te_delete(_collisionConfiguration);
        te_delete(_broadphase);
        te_delete(_worldInfo);
    }

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
