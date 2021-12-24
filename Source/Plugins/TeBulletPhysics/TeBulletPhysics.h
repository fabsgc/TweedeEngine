#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysics.h"
#include "Physics/TePhysicsCommon.h"
#include "Utility/TePoolAllocator.h"
#include "TeBulletMesh.h"

namespace te 
{
    class BulletScene;

    /** Bullet implementation of Physics. */
    class BulletPhysics : public Physics
    {
    public:
        /** Type of contacts reported by PhysX simulation. */
        enum class ContactEventType
        {
            ContactBegin,
            ContactStay,
            ContactEnd
        };

        /** Event reported when two colliders interact. */
        struct ContactEvent
        {
            Body* BodyA = nullptr; /** First body. */
            Body* BodyB = nullptr; /** Second body. */
            ContactEventType Type = ContactEventType::ContactBegin; /** Exact type of the event. */
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

        /** @copydoc Physics::CreateMesh */
        SPtr<PhysicsMesh> CreateMesh(const SPtr<MeshData>& meshData) override;

        /** @copydoc Physics::CreateMesh */
        SPtr<PhysicsHeightField> CreateHeightField(const SPtr<Texture>& texture) override;

        /** @copydoc Physics::CreatePhysicsScene */
        SPtr<PhysicsScene> CreatePhysicsScene() override;

        /** @copydoc Physics::Update */
        void Update() override;

        /** @copydoc Physics::SetPaused */
        void SetPaused(bool paused) override;

        /** @copydoc Physics::SetPaused */
        bool IsPaused() const override;

        /** Enable or disable debug informations globally */
        void SetDebug(bool debug) override;

        /** @copydoc SetDebug */
        bool IsDebug() override;

        /** @copydoc Physics::DrawDebug */
        void DrawDebug(const SPtr<Camera>& camera, const SPtr<RenderTarget>& renderTarget) override;

        /** @copydoc Physics::SetGravity */
        virtual void SetGravity(const Vector3& gravity) override;

        /** @copydoc Physics::SetAirDensity */
        virtual void SetAirDensity(const float& airDensity) override;

        /** @copydoc Physics::SetWaterDensity */
        virtual void SetWaterDensity(const float& waterDensity) override;

        /** @copydoc Physics::SetWaterNormal */
        virtual void SetWaterNormal(const Vector3& waterDensity) override;

        /** @copydoc Physics::SetWaterOffset */
        virtual void SetWaterOffset(const float& waterOffset) override;

        /** Notifies the system that at physics scene is about to be destroyed. */
        void NotifySceneDestroyed(BulletScene* scene);

    private:
        friend class BulletScene;

        bool _paused; // is simulation paused
        bool _debug; // is debug enabled

        btBroadphaseInterface* _broadphase = nullptr;
        btCollisionDispatcher* _collisionDispatcher = nullptr;
        btSequentialImpulseConstraintSolver* _constraintSolver = nullptr;
        btDefaultCollisionConfiguration* _collisionConfiguration = nullptr;

        Vector<BulletScene*> _scenes;

        INT32 _maxSubSteps = 1;
        UINT32 _maxSolveIterations = 256;
        float _internalFps = 60.0f;
        float _deltaTimeSec = 1.0f;

        UINT32 _debugMode = btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawContactPoints | 
            btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits;
    };

    typedef Pair<const btCollisionObject*, const btCollisionObject*> ContactEventKey;

    struct ContactEventKeyHash
    {
        template <class T1, class T2>
        std::size_t operator() (const Pair<T1, T2>& pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

    typedef UnorderedMap<ContactEventKey, BulletPhysics::ContactEvent*, ContactEventKeyHash> ContactEventsMap;

    /** Contains information about a single Bullet scene. */
    class BulletScene : public PhysicsScene
    {
    public:
        BulletScene(BulletPhysics* physics, const PHYSICS_INIT_DESC& desc);
        ~BulletScene();

        /** PhysicsScene::TriggerCollisions */
        void TriggerCollisions() override;

        /** PhysicsScene::TriggerCollisions */
        void ReportCollisions() override;

        /** @copydoc PhysicsScene::CreateRigidBody */
        SPtr<RigidBody> CreateRigidBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreateSoftBody */
        SPtr<SoftBody> CreateSoftBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreateMeshSoftBody */
        SPtr<MeshSoftBody> CreateMeshSoftBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreateEllipsoidSoftBody */
        SPtr<EllipsoidSoftBody> CreateEllipsoidSoftBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreateRopeSoftBody */
        SPtr<RopeSoftBody> CreateRopeSoftBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreatePatchSoftBody */
         SPtr<PatchSoftBody> CreatePatchSoftBody(const HSceneObject& linkedSO) override;

        /** @copydoc PhysicsScene::CreateConeTwistJoint */
        SPtr<ConeTwistJoint> CreateConeTwistJoint() override;

        /** @copydoc PhysicsScene::CreateHingeJoint */
        SPtr<HingeJoint> CreateHingeJoint() override;

        /** @copydoc PhysicsScene::CreateSphericalJoint */
        SPtr<SphericalJoint> CreateSphericalJoint() override;

        /** @copydoc PhysicsScene::CreateSliderJoint */
        SPtr<SliderJoint> CreateSliderJoint() override;

        /** @copydoc PhysicsScene::CreateD6Joint */
        SPtr<D6Joint> CreateD6Joint() override;

        /** @copydoc PhysicsScene::CreateBoxCollider */
        SPtr<BoxCollider> CreateBoxCollider(const Vector3& extents, const Vector3& position,
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreatePlaneCollider */
        SPtr<PlaneCollider> CreatePlaneCollider(const Vector3& normal, const Vector3& position,
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreateSphereCollider */
        SPtr<SphereCollider> CreateSphereCollider(float radius, const Vector3& position, 
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreateCylinderCollider */
        SPtr<CylinderCollider> CreateCylinderCollider(const Vector3& extents, const Vector3& position,
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreateCapsuleCollider */
        SPtr<CapsuleCollider> CreateCapsuleCollider(float radius, float height, const Vector3& position,
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreateMeshCollider */
        SPtr<MeshCollider> CreateMeshCollider(const Vector3& position, const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreateConeCollider */
        SPtr<ConeCollider> CreateConeCollider(float radius, float height, const Vector3& position,
            const Quaternion& rotation) override;

        /** @copydoc PhysicsScene::CreateHeightFieldCollider */
        SPtr<HeightFieldCollider> CreateHeightFieldCollider(const Vector3& position, const Quaternion& rotation) override;

        /** Add RigidBody to current scene */
        void AddRigidBody(btRigidBody* body);

        /** Remove RigidBody from scene */
        void RemoveRigidBody(btRigidBody* body);

        /** Add SoftBody to current scene */
        void AddSoftBody(btSoftBody* body);

        /** Remove SoftBody from scene */
        void RemoveSoftBody(btSoftBody* body);

        /** Add Joint to the current scene */
        void AddJoint(btTypedConstraint* joint, bool collisionWithLinkedBody = true) const;

        /** Remove Joint from the current scene */
        void RemoveJoint(btTypedConstraint* joint) const;

        /** @copydoc Physics::RayCast */
        bool RayCast(const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit,
            float maxDist = FLT_MAX) const override;

        /** @copydoc Physics::RayCast */
        bool RayCast(const Vector3& origin, const Vector3& unitDir, Vector<PhysicsQueryHit>& hits,
            float maxDist = FLT_MAX) const override;

        /** Create a btSoftBody from a PhysicsMesh */
        btSoftBody* CreateBtSoftBody(const SPtr<BulletMesh::MeshInfo>& mesh) const;

    private:
        friend class BulletPhysics;

        PHYSICS_INIT_DESC _initDesc;
        BulletPhysics* _physics = nullptr;

        btDiscreteDynamicsWorld* _world = nullptr;
        btSoftBodyWorldInfo* _worldInfo = nullptr;

        ContactEventsMap* _beginContactEvents = nullptr;
        ContactEventsMap* _stayContactEvents = nullptr;
        ContactEventsMap* _endContactEvents = nullptr;
    };

    BulletPhysics& gBulletPhysics();

    IMPLEMENT_GLOBAL_POOL(BulletPhysics::ContactEvent, 64)
}
