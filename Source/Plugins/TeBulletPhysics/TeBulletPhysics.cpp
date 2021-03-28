#include "TeBulletPhysics.h"
#include "TeCoreApplication.h"
#include "TeBulletRigidBody.h"
#include "TeBulletSoftBody.h"

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
        }
        else
        {
            // Create
            _collisionConfiguration = te_new<btDefaultCollisionConfiguration>();
            _collisionDispatcher = te_new<btCollisionDispatcher>(_collisionConfiguration);
        }
    }

    BulletPhysics::~BulletPhysics()
    { 
        assert(_scenes.empty() && "All scenes must be freed before physics system shutdown");

        te_delete(_constraintSolver);
        te_delete(_collisionDispatcher);
        te_delete(_collisionConfiguration);
        te_delete(_broadphase);
    }

    void BulletPhysics::SetPaused(bool paused)
    {
        _paused = paused;
    }

    bool BulletPhysics::IsPaused() const
    {
        return _paused;
    }

    SPtr<PhysicsScene> BulletPhysics::CreatePhysicsScene()
    {
        SPtr<BulletScene> scene = te_shared_ptr_new<BulletScene>(this, _initDesc);
        _scenes.push_back(scene.get());

        return scene;
    }

    void BulletPhysics::NotifySceneDestroyed(BulletScene* scene)
    {
        auto iterFind = std::find(_scenes.begin(), _scenes.end(), scene);
        assert(iterFind != _scenes.end());

        _scenes.erase(iterFind);
    }

    void BulletPhysics::FixedUpdate(float step)
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Physics);
        if (IsPaused() || !isRunning)
            return;

        for (auto& scene : _scenes)
        {

        }
    }

    void BulletPhysics::Update()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Physics);
        if (IsPaused() || !isRunning)
            return;

        for (auto& scene : _scenes)
        {

        }
    }

    BulletScene::BulletScene(BulletPhysics* physics, const PHYSICS_INIT_DESC& desc)
        : PhysicsScene()
        , _initDesc(desc)
        , _physics(physics)
    {
        if (_initDesc.SoftBody)
        {
            _world = te_new<btSoftRigidDynamicsWorld>(_physics->_collisionDispatcher, _physics->_broadphase,
                _physics->_constraintSolver, _physics->_collisionConfiguration);

            // Setup         
            _worldInfo = te_new<btSoftBodyWorldInfo>();
            _worldInfo->m_sparsesdf.Initialize();

            _world->getDispatchInfo().m_enableSPU = true;
            _worldInfo->m_dispatcher = _physics->_collisionDispatcher;
            _worldInfo->m_broadphase = _physics->_broadphase;
            _worldInfo->air_density = (btScalar)_initDesc.AirDensity;
            _worldInfo->water_density = 0;
            _worldInfo->water_offset = 0;
            _worldInfo->water_normal = btVector3(0, 0, 0);
            _worldInfo->m_gravity = ToBtVector3(_initDesc.Gravity);
        }
        else
        {
            _world = te_new<btDiscreteDynamicsWorld>(_physics->_collisionDispatcher, _physics->_broadphase,
                _physics->_constraintSolver, _physics->_collisionConfiguration);
        }

        // Setup
        _world->setGravity(ToBtVector3(_initDesc.Gravity));
        _world->getDispatchInfo().m_useContinuous = true;
        _world->getSolverInfo().m_splitImpulse = false;
        _world->getSolverInfo().m_numIterations = _physics->_maxSolveIterations;
    }

    BulletScene::~BulletScene()
    {
        te_safe_delete(_world);
        te_safe_delete(_worldInfo);

        gBulletPhysics().NotifySceneDestroyed(this);
    }

    SPtr<RigidBody> BulletScene::CreateRigidBody(const HSceneObject& linkedSO)
    {
        return te_shared_ptr_new<BulletRigidBody>(_physics, this, linkedSO);
    }

    SPtr<SoftBody> BulletScene::CreateSoftBody(const HSceneObject& linkedSO)
    {
        return te_shared_ptr_new<BulletSoftBody>(_physics, this, linkedSO);
    }

    BulletPhysics& gBulletPhysics()
    {
        return static_cast<BulletPhysics&>(BulletPhysics::Instance());
    }
}
