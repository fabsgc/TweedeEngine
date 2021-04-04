#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysics.h"
#include "Physics/TePhysicsCommon.h"

namespace te 
{
    class BulletScene;

    /** Bullet implementation of Physics. */
    class BulletPhysics : public Physics
    {
    public:
        BulletPhysics(const PHYSICS_INIT_DESC& input);
        ~BulletPhysics();

        TE_MODULE_STATIC_HEADER_MEMBER(BulletPhysics)

        /** @copydoc Physics::CreatePhysicsScene */
        SPtr<PhysicsScene> CreatePhysicsScene() override;

        /** @copydoc Physics::FixedUpdate */
        void FixedUpdate(float step) override;

        /** @copydoc Physics::Update */
        void Update() override;

        /** @copydoc Physics::SetPaused */
        void SetPaused(bool paused) override;

        /** @copydoc Physics::SetPaused */
        bool IsPaused() const override;

        /** Notifies the system that at physics scene is about to be destroyed. */
        void NotifySceneDestroyed(BulletScene* scene);

    private:
        friend class BulletScene;

        PHYSICS_INIT_DESC _initDesc;
        bool _paused;

        btBroadphaseInterface* _broadphase = nullptr;
        btCollisionDispatcher* _collisionDispatcher = nullptr;
        btSequentialImpulseConstraintSolver* _constraintSolver = nullptr;
        btDefaultCollisionConfiguration* _collisionConfiguration = nullptr;

        Vector<PhysicsScene*> _scenes;

        UINT32 _maxSubSteps = 1;
        UINT32 _maxSolveIterations = 256;
        float _internalFps = 60.0f;
    };

    /** Contains information about a single PhysX scene. */
    class BulletScene : public PhysicsScene
    {
    public:
        BulletScene(BulletPhysics* physics, const PHYSICS_INIT_DESC& desc);
        ~BulletScene();

        /** @copydoc PhysicsScene::CreateRigidBody */
        SPtr<RigidBody> CreateRigidBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreateSoftBody */
        SPtr<SoftBody> CreateSoftBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreateSoftBody */
        SPtr<ConeTwistJoint> CreateConeTwistJoint(const CONE_TWIST_JOINT_DESC& desc) override;

        /** @copydoc PhysicsScene::CreateSoftBody */
        SPtr<HingeJoint> CreateHingeJoint(const HINGE_JOINT_DESC& desc) override;

        /** @copydoc PhysicsScene::CreateSoftBody */
        SPtr<SphericalJoint> CreateSphericalJoint(const SPHERICAL_JOINT_DESC& desc) override;

        /** @copydoc PhysicsScene::CreateSoftBody */
        SPtr<SliderJoint> CreateSliderJoint(const SLIDER_JOINT_DESC& desc) override;

        /** @copydoc PhysicsScene::CreateSoftBody */
        SPtr<D6Joint> CreateD6Joint(const D6_JOINT_DESC& desc) override;

        /** @copydoc PhysicsScene::CreateBoxCollider */
        SPtr<BoxCollider> CreateBoxCollider();

        /** @copydoc PhysicsScene::CreatePlaneCollider */
        SPtr<PlaneCollider> CreatePlaneCollider();

        /** @copydoc PhysicsScene::CreateSphereCollider */
        SPtr<SphereCollider> CreateSphereCollider();

        /** @copydoc PhysicsScene::CreateCylinderCollider */
        SPtr<CylinderCollider> CreateCylinderCollider();

        /** @copydoc PhysicsScene::CreateCapsuleCollider */
        SPtr<CapsuleCollider> CreateCapsuleCollider();

        /** @copydoc PhysicsScene::CreateMeshCollider */
        SPtr<MeshCollider> CreateMeshCollider();

        /** @copydoc PhysicsScene::CreateConeCollider */
        SPtr<ConeCollider> CreateConeCollider();

    private:
        friend class BulletPhysics;

        PHYSICS_INIT_DESC _initDesc;

        BulletPhysics* _physics = nullptr;

        btDiscreteDynamicsWorld* _world = nullptr;
        btSoftBodyWorldInfo* _worldInfo = nullptr;
    };

    BulletPhysics& gBulletPhysics();
}
