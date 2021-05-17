#include "TeBulletPhysics.h"
#include "TeCoreApplication.h"
#include "TeBulletRigidBody.h"
#include "TeBulletSoftBody.h"
#include "TeBulletConeTwistJoint.h"
#include "TeBulletD6Joint.h"
#include "TeBulletHingeJoint.h"
#include "TeBulletSliderJoint.h"
#include "TeBulletSphericalJoint.h"
#include "TeBulletBoxCollider.h"
#include "TeBulletPlaneCollider.h"
#include "TeBulletSphereCollider.h"
#include "TeBulletCylinderCollider.h"
#include "TeBulletCapsuleCollider.h"
#include "TeBulletMeshCollider.h"
#include "TeBulletConeCollider.h"
#include "TeBulletHeightFieldCollider.h"
#include "TeBulletDebug.h"
#include "TeBulletMesh.h"
#include "TeBulletHeightField.h"
#include "Utility/TeTime.h"
#include "RenderAPI/TeRenderAPI.h"
#include "RenderAPI/TeRenderTexture.h"

using namespace std::placeholders;

namespace te
{
    TE_MODULE_STATIC_MEMBER(BulletPhysics)

    BulletPhysics::BulletPhysics(const PHYSICS_INIT_DESC& desc)
        : Physics(desc)
        , _paused(false)
        , _debug(true)
    {
        _broadphase = te_new<btDbvtBroadphase>();
        _constraintSolver = te_new<btSequentialImpulseConstraintSolver>();

        if (_initDesc.SoftBody)
        {
            _collisionConfiguration = te_new<btSoftBodyRigidBodyCollisionConfiguration>();
            _collisionDispatcher = te_new<btCollisionDispatcher>(_collisionConfiguration);
            //btGImpactCollisionAlgorithm::registerAlgorithm(_collisionDispatcher);
        }
        else
        {
            _collisionConfiguration = te_new<btDefaultCollisionConfiguration>();
            _collisionDispatcher = te_new<btCollisionDispatcher>(_collisionConfiguration);
            //btGImpactCollisionAlgorithm::registerAlgorithm(_collisionDispatcher);
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

    void BulletPhysics::SetDebug(bool debug)
    {
        _debug = debug;
    }

    bool BulletPhysics::IsDebug()
    {
        return _debug;
    }

    SPtr<PhysicsMesh> BulletPhysics::CreateMesh(const SPtr<MeshData>& meshData)
    {
        return te_core_ptr_new<BulletMesh>(meshData);
    }

    SPtr<PhysicsHeightField> BulletPhysics::CreateHeightField(const SPtr<Texture>& texture)
    {
        return te_core_ptr_new<BulletHeightField>(texture);
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

    void BulletPhysics::Update()
    {
        for (auto& scene : _scenes)
        {
            if (scene->_world && scene->_debug)
            {
                scene->_debug->Clear();

                if(_debug)
                    scene->_world->debugDrawWorld();
            }
        }

        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Physics);
        if (IsPaused() || !isRunning)
            return;

        for (auto& scene : _scenes)
        {
            if (!scene->_world)
                continue;

            // This equation must be met: timeStep < maxSubSteps * fixedTimeStep
            float deltaTimeSec = gTime().GetFrameDelta();
            float internalTimeStep = 1.0f / _internalFps;
            INT32 maxSubsteps = static_cast<INT32>(deltaTimeSec * _internalFps) + 1;
            if (_maxSubSteps < 0)
            {
                internalTimeStep = deltaTimeSec;
                maxSubsteps = 1;
            }
            else if (_maxSubSteps > 0)
            {
                maxSubsteps = std::min<INT32>(maxSubsteps, _maxSubSteps);
            }

            // Step the physics world.
            _updateInProgress = true;
            scene->_world->stepSimulation(deltaTimeSec, maxSubsteps, internalTimeStep);
            _updateInProgress = false;

            _deltaTimeSec += deltaTimeSec;
            if (_deltaTimeSec > 1.0f / _internalFps)
            {
                scene->TriggerCollisions();
                scene->ReportCollisions();
                _deltaTimeSec = 0.0f;
            }
        }
    }

    void BulletPhysics::DrawDebug(const SPtr<Camera>& camera, const SPtr<RenderTarget>& renderTarget)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_DEPTH;

        rapi.SetRenderTarget(renderTarget);
        rapi.ClearViewport(clearBuffers, Color::Black);

        for (auto& scene : _scenes)
        {
            if (!scene->_world || !scene->_debug)
                continue;

            scene->_debug->Draw(camera, renderTarget);
        }

        rapi.SetRenderTarget(nullptr);
    }

    void BulletPhysics::SetGravity(const Vector3& gravity)
    {
        _initDesc.Gravity = gravity;

        for (auto& scene : _scenes)
        {
            if (scene->_world)
                scene->_world->setGravity(ToBtVector3(gravity));
        }
    }

    using bp = BulletPhysics;

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
            _worldInfo->water_density = (btScalar)_initDesc.WaterDensity;
            _worldInfo->water_offset = (btScalar)_initDesc.WaterOffset;
            _worldInfo->water_normal = ToBtVector3(_initDesc.WaterNormal);
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

#if TE_PLATFORM == TE_PLATFORM_WIN32
        _debug = te_new<BulletDebug>();
        ((BulletDebug*)_debug)->setDebugMode(_physics->_debugMode);
        _world->setDebugDrawer(static_cast<BulletDebug*>(_debug));
#endif

        _beginContactEvents = te_new<ContactEventsMap>();
        _stayContactEvents = te_new<ContactEventsMap>();
        _endContactEvents = te_new<ContactEventsMap>();
    }

    BulletScene::~BulletScene()
    {
        te_safe_delete(_world);
        te_safe_delete(_worldInfo);
        te_safe_delete((BulletDebug*)_debug);

        for (auto& evt : *_beginContactEvents)
            te_pool_delete(evt.second);

        for (auto& evt : *_stayContactEvents)
            te_pool_delete(evt.second);

        for (auto& evt : *_endContactEvents)
            te_pool_delete(evt.second);

        _beginContactEvents->clear();
        _stayContactEvents->clear();
        _endContactEvents->clear();

        te_delete(_beginContactEvents);
        te_delete(_stayContactEvents);
        te_delete(_endContactEvents);

        gBulletPhysics().NotifySceneDestroyed(this);
    }

    void BulletScene::TriggerCollisions()
    {
        Body* bodyA = nullptr;
        Body* bodyB = nullptr;
        bp::ContactEvent* currContactEvent = nullptr;

        if (_world)
            _world->performDiscreteCollisionDetection();

        int numManifolds = _world->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold = _world->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = contactManifold->getBody0();
            const btCollisionObject* obB = contactManifold->getBody1();
            bodyA = nullptr;
            bodyB = nullptr;

            auto stayContactEvent = _stayContactEvents->find(std::make_pair(obA, obB));
            if (stayContactEvent != _stayContactEvents->end())
            {
                stayContactEvent->second->Type = bp::ContactEventType::ContactStay;
                stayContactEvent->second->Points.clear();

                currContactEvent = stayContactEvent->second;
            }
            else
            {
                bodyA = (obA->getUserPointer()) ? static_cast<Body*>(obA->getUserPointer()) : nullptr;
                bodyB = (obB->getUserPointer()) ? static_cast<Body*>(obB->getUserPointer()) : nullptr;

                if (!bodyA || !bodyB)
                    continue;

                if (bodyA->GetCollisionReportMode() == CollisionReportMode::None
                    && bodyB->GetCollisionReportMode() == CollisionReportMode::None)
                    continue;

                bp::ContactEvent* beginEvent = te_pool_new<bp::ContactEvent>();
                beginEvent->Type = bp::ContactEventType::ContactBegin;
                beginEvent->BodyA = bodyA;
                beginEvent->BodyB = bodyA;

                _beginContactEvents->insert(Pair<ContactEventKey, bp::ContactEvent*>(
                    std::make_pair(obA, obB), beginEvent));

                currContactEvent = beginEvent;
            }

            int numContacts = contactManifold->getNumContacts();
            for (int j = 0; j < numContacts; j++)
            {
                btManifoldPoint& pt = contactManifold->getContactPoint(j);

                if (currContactEvent)
                {
                    ContactPoint point;

                    point.PositionA = ToVector3(pt.getPositionWorldOnA());
                    point.PositionB = ToVector3(pt.getPositionWorldOnB());
                    point.Normal = ToVector3(pt.m_normalWorldOnB);
                    point.Impulse = (float)pt.getAppliedImpulse();
                    point.Distance = (float)pt.getDistance();

                    currContactEvent->Points.push_back(point);
                }
            }
        }

        for (auto it = _stayContactEvents->begin(); it != _stayContactEvents->end(); )
        {
            if (it->second->Type == bp::ContactEventType::ContactBegin)
            {
                _endContactEvents->insert(Pair<ContactEventKey, bp::ContactEvent*>(
                    std::make_pair(it->first.first, it->first.second), it->second));

                it->second->Type = bp::ContactEventType::ContactEnd;
                it = _stayContactEvents->erase(it);
                continue;
            }

            it++;
        }
    }

    void BulletScene::ReportCollisions()
    {
        CollisionDataRaw data;

        auto NotifyContact = [&](Body* body, bp::ContactEvent* evt)
        {
            CollisionReportMode mode = body->GetCollisionReportMode();
            if (mode == CollisionReportMode::None)
                return;

            data.Bodies[0] = evt->BodyA;
            data.Bodies[1] = evt->BodyB;
            data.ContactPoints = evt->Points;

            switch (evt->Type)
            {
            case bp::ContactEventType::ContactBegin:
                body->OnCollisionBegin(data);
                break;
            case bp::ContactEventType::ContactStay:
                if(mode == CollisionReportMode::ReportPersistent)
                    body->OnCollisionStay(data);
                break;
            case bp::ContactEventType::ContactEnd:
                body->OnCollisionEnd(data);
                break;
            }
        };

        for (auto& evt : *_stayContactEvents)
        {
            NotifyContact(evt.second->BodyA, evt.second);
            evt.second->Type = bp::ContactEventType::ContactBegin;
        }

        for (auto& evt : *_beginContactEvents)
        {
            NotifyContact(evt.second->BodyA, evt.second);
            _stayContactEvents->insert(Pair<ContactEventKey, bp::ContactEvent*>(
                std::make_pair(evt.first.first, evt.first.second), evt.second));
        }

        for (auto& evt : *_endContactEvents)
        {
            NotifyContact(evt.second->BodyA, evt.second);
            te_pool_delete(evt.second);
        }

        _endContactEvents->clear();
        _beginContactEvents->clear();
    }

    SPtr<RigidBody> BulletScene::CreateRigidBody(const HSceneObject& linkedSO)
    {
        return te_shared_ptr_new<BulletRigidBody>(_physics, this, linkedSO);
    }

    SPtr<SoftBody> BulletScene::CreateSoftBody(const HSceneObject& linkedSO)
    {
        return te_shared_ptr_new<BulletSoftBody>(_physics, this, linkedSO);
    }

    SPtr<ConeTwistJoint> BulletScene::CreateConeTwistJoint()
    {
        return te_shared_ptr_new<BulletConeTwistJoint>(_physics, this);
    }

    SPtr<HingeJoint> BulletScene::CreateHingeJoint()
    {
        return te_shared_ptr_new<BulletHingeJoint>(_physics, this);
    }

    SPtr<SphericalJoint> BulletScene::CreateSphericalJoint()
    {
        return te_shared_ptr_new<BulletSphericalJoint>(_physics, this);
    }

    SPtr<SliderJoint> BulletScene::CreateSliderJoint()
    {
        return te_shared_ptr_new<BulletSliderJoint>(_physics, this);
    }

    SPtr<D6Joint> BulletScene::CreateD6Joint()
    {
        return te_shared_ptr_new<BulletD6Joint>(_physics, this);
    }

    SPtr<BoxCollider> BulletScene::CreateBoxCollider(const Vector3& extents, const Vector3& position,
        const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletBoxCollider>(_physics, this, position, rotation, extents);
    }

    SPtr<PlaneCollider> BulletScene::CreatePlaneCollider(const Vector3& normal, const Vector3& position,
        const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletPlaneCollider>(_physics, this, position, rotation, normal);
    }

    SPtr<SphereCollider> BulletScene::CreateSphereCollider(float radius, const Vector3& position, const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletSphereCollider>(_physics, this, position, rotation, radius);
    }

    SPtr<CylinderCollider> BulletScene::CreateCylinderCollider(const Vector3& extents, const Vector3& position,
        const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletCylinderCollider>(_physics, this, position, rotation, extents);
    }

    SPtr<CapsuleCollider> BulletScene::CreateCapsuleCollider(float radius, float height, const Vector3& position,
        const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletCapsuleCollider>(_physics, this, position, rotation, radius, height);
    }

    SPtr<MeshCollider> BulletScene::CreateMeshCollider(const Vector3& position, const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletMeshCollider>(_physics, this, position, rotation);
    }

    SPtr<ConeCollider> BulletScene::CreateConeCollider(float radius, float height, const Vector3& position,
        const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletConeCollider>(_physics, this, position, rotation, radius, height);
    }

    SPtr<HeightFieldCollider> BulletScene::CreateHeightFieldCollider(const Vector3& position, const Quaternion& rotation)
    {
        return te_shared_ptr_new<BulletHeightFieldCollider>(_physics, this, position, rotation);
    }

    void BulletScene::AddRigidBody(btRigidBody* body)
    {
        if (!_world)
            return;

        _world->addRigidBody(body);
    }

    void BulletScene::RemoveRigidBody(btRigidBody* body)
    {
        if (!_world)
            return;

        for (auto it = _stayContactEvents->begin(); it != _stayContactEvents->end();)
        {
            const btCollisionObject* objA = it->first.first;
            const btCollisionObject* objB = it->first.second;

            if (objA == body || objB == body)
            {
                te_pool_delete(it->second);
                it = _stayContactEvents->erase(it);
                continue;
            }

            it++;
        }

        _world->removeRigidBody(body);
    }

    BulletPhysics& gBulletPhysics()
    {
        return static_cast<BulletPhysics&>(BulletPhysics::Instance());
    }
}
