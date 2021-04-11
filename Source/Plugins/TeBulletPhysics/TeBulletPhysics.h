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
        /** Type of contacts reported by PhysX simulation. */
        enum class ContactEventType
        {
            ContactBegin,
            ContactStay,
            ContactEnd
        };

        /** Event reported when a physics object interacts with a collider. */
        struct TriggerEvent
        {
            Collider* Trigger; /** Trigger that was interacted with. */
            Collider* Other; /** Collider that was interacted with. */
            ContactEventType Type; /** Exact type of the event. */
        };

        /** Event reported when two colliders interact. */
        struct ContactEvent
        {
            Collider* ColliderA; /** First collider. */
            Collider* ColliderB; /** Second collider. */
            ContactEventType Type; /** Exact type of the event. */
            // Note: Not too happy this is heap allocated, use static allocator?
            Vector<ContactPoint> Points; /** Information about all contact points between the colliders. */
        };

        /** Event reported when a joint breaks. */
        struct JointBreakEvent
        {
            Joint* JointElt; /** Broken joint. */
        };

    public:
        BulletPhysics(const PHYSICS_INIT_DESC& input);
        ~BulletPhysics();

        TE_MODULE_STATIC_HEADER_MEMBER(BulletPhysics)

        /** @copydoc Physics::CreatePhysicsScene */
        SPtr<PhysicsScene> CreatePhysicsScene() override;

        /** @copydoc Physics::Update */
        void Update() override;

        /** @copydoc Physics::SetPaused */
        void SetPaused(bool paused) override;

        /** @copydoc Physics::SetPaused */
        bool IsPaused() const override;

        /** @copydoc Physics::DrawDebug */
        void DrawDebug(const SPtr<RenderTarget>& renderTarget) override;

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

        Vector<BulletScene*> _scenes;

        UINT32 _maxSubSteps = 1;
        UINT32 _maxSolveIterations = 256;
        float _internalFps = 60.0f;
        float _deltaTimeSec = 1.0f;
    };

    /** Contains information about a single Bullet scene. */
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
        SPtr<BoxCollider> CreateBoxCollider(const Vector3& extents, const Vector3& position,
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreatePlaneCollider */
        SPtr<PlaneCollider> CreatePlaneCollider() override;

        /** @copydoc PhysicsScene::CreateSphereCollider */
        SPtr<SphereCollider> CreateSphereCollider(float radius, const Vector3& position, 
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreateCylinderCollider */
        SPtr<CylinderCollider> CreateCylinderCollider() override;

        /** @copydoc PhysicsScene::CreateCapsuleCollider */
        SPtr<CapsuleCollider> CreateCapsuleCollider() override;

        /** @copydoc PhysicsScene::CreateMeshCollider */
        SPtr<MeshCollider> CreateMeshCollider() override;

        /** @copydoc PhysicsScene::CreateConeCollider */
        SPtr<ConeCollider> CreateConeCollider() override;

        /** Add RigidBody to current scene */
        void AddRigidBody(btRigidBody* body);

        /** Remove RigidBody from scene */
        void RemoveRigidBody(btRigidBody* body);

    private:
        friend class BulletPhysics;

        PHYSICS_INIT_DESC _initDesc;

        BulletPhysics* _physics = nullptr;

        btDiscreteDynamicsWorld* _world = nullptr;
        btSoftBodyWorldInfo* _worldInfo = nullptr;
    };

    BulletPhysics& gBulletPhysics();
}
