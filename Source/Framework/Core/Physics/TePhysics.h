#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TePhysicsDebug.h"
#include "Utility/TeModule.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

#include <cfloat>

namespace te
{
    /** Contains parameters used for initializing the physics system. */
    struct PHYSICS_INIT_DESC
    {
        Vector3 Gravity = Vector3(0.0f, -9.81f, 0.0f); /**< Initial gravity. */
        float AirDensity = 1.2f;
        float WaterDensity = 1.5f;
        float WaterOffset = 0.0f;
        Vector3 WaterNormal = Vector3(0.0f, 1.0f, 0.0f);
        bool SoftBody = false;
    };

    /** Provides global physics settings, factory methods for physics objects and scene queries. */
    class TE_CORE_EXPORT Physics : public Module<Physics>
    {
    public:
        Physics(const PHYSICS_INIT_DESC& init);
        virtual ~Physics() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(Physics)

        /** Creates an object representing the physics scene. Must be manually released via destroyPhysicsScene(). */
        virtual SPtr<PhysicsScene> CreatePhysicsScene() = 0;

        /** Performs any physics operations that arent tied to the fixed update interval. Should be called once per frame. */
        virtual void Update() { }

        /** Determines if audio reproduction is paused globally. */
        virtual void SetPaused(bool paused) = 0;

        /** @copydoc SetPaused() */
        virtual bool IsPaused() const = 0;

        /** Allow user to see debug information about physical simulation */
        virtual void DrawDebug(const SPtr<RenderTarget>& renderTarget) = 0;

        /** Checks is the physics simulation update currently in progress. */
        bool IsUpdateInProgress() const { return _updateInProgress; }

        /** Returns configuration of the physic engine */
        const PHYSICS_INIT_DESC& GetDesc() const { return _initDesc; }

    protected:
        bool _updateInProgress = false;
        PHYSICS_INIT_DESC _initDesc;
    };

    /**
     * Physical representation of a scene, allowing creation of new physical objects in the scene and queries against
     * those objects. Objects created in different scenes cannot physically interact with eachother.
     */
    class TE_CORE_EXPORT PhysicsScene
    {
    public:
        /** @copydoc RigidBody::Create */
        virtual SPtr<RigidBody> CreateRigidBody(const HSceneObject& linkedSO) = 0;

        /** @copydoc RigidBody::Create */
        virtual SPtr<SoftBody> CreateSoftBody(const HSceneObject& linkedSO) = 0;

        /**
         * Creates a new cone twist joint.
         *
         * @param[in]	desc		Settings describing the joint.
         */
        virtual SPtr<ConeTwistJoint> CreateConeTwistJoint(const CONE_TWIST_JOINT_DESC& desc) = 0;

        /**
         * Creates a new hinge joint.
         *
         * @param[in]	desc		Settings describing the joint.
         */
        virtual SPtr<HingeJoint> CreateHingeJoint(const HINGE_JOINT_DESC& desc) = 0;

        /**
         * Creates a new spherical joint.
         *
         * @param[in]	desc		Settings describing the joint.
         */
        virtual SPtr<SphericalJoint> CreateSphericalJoint(const SPHERICAL_JOINT_DESC& desc) = 0;

        /**
         * Creates a new spherical joint.
         *
         * @param[in]	desc		Settings describing the joint.
         */
        virtual SPtr<SliderJoint> CreateSliderJoint(const SLIDER_JOINT_DESC& desc) = 0;

        /**
         * Creates a new D6 joint.
         *
         * @param[in]	desc		Settings describing the joint.
         */
        virtual SPtr<D6Joint> CreateD6Joint(const D6_JOINT_DESC& desc) = 0;

        /**
         * Creates a new box collider.
         * 
         * @param[in]	extents		Extents (half size) of the box.
		 * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        virtual SPtr<BoxCollider> CreateBoxCollider(const Vector3& extents, const Vector3& position,
            const Quaternion& rotation) = 0;

        /**
         * Creates a new plane collider.
         * 
		 * @param[in]	normal      Normal to the plane.
         * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        virtual SPtr<PlaneCollider> CreatePlaneCollider(const Vector3& normal, const Vector3& position,
            const Quaternion& rotation) = 0;

        /**
         * Creates a new sphere collider.
         * 
         * @param[in]	radius		Radius of the sphere geometry.
		 * @param[in]	position	Position of the collider.
		 * @param[in]	rotation	Rotation of the collider.
         */
        virtual SPtr<SphereCollider> CreateSphereCollider(float radius, const Vector3& position,
            const Quaternion& rotation) = 0;

        /**
         * Creates a new cylinder collider.
         * 
         * @param[in]	extents		Extents (half size) of the cylinder.
		 * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        virtual SPtr<CylinderCollider> CreateCylinderCollider(const Vector3& extents, const Vector3& position,
            const Quaternion& rotation) = 0;

        /**
         * Creates a new capsule collider.
         * 
         * @param[in]	radius		Radius of the capsule
         * @param[in]	height		Height of the capsule
		 * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        virtual SPtr<CapsuleCollider> CreateCapsuleCollider(float radius, float height, const Vector3& position,
            const Quaternion& rotation) = 0;

        /**
         * Creates a new mesh collider.
         * 
         * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        virtual SPtr<MeshCollider> CreateMeshCollider(const Vector3& position, const Quaternion& rotation) = 0;

        /**
         * Creates a new cone collider.
         * 
         * @param[in]	radius		Radius of the cone
         * @param[in]	height		Height of the cone
		 * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        virtual SPtr<ConeCollider> CreateConeCollider(float radius, float height, const Vector3& position,
            const Quaternion& rotation) = 0;

    protected:
        PhysicsScene() = default;
        virtual ~PhysicsScene() = default;

    protected:
        PhysicsDebug* _debug = nullptr;

    };

    /** Provides easier access to Physics. */
    TE_CORE_EXPORT Physics& gPhysics();
}
