#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Utility/TeModule.h"
#include "Math/TeVector3.h"

#include <cfloat>

namespace te
{
    /** Contains parameters used for initializing the physics system. */
    struct PHYSICS_INIT_DESC
    {
        float TypicalLength = 1.0f; /**< Typical length of an object in the scene. */
        float TypicalSpeed = 9.81f; /**< Typical speed of an object in the scene. */
        Vector3 Gravity = Vector3(0.0f, -9.81f, 0.0f); /**< Initial gravity. */
        float AirDensity = 1.2f;
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

        /**
         * Updates the physics simulation. In order to maintain stability of the physics calculations this method should
         * be called at fixed intervals (e.g. 60 times a second).
         *
         * @param[in]	step	Time delta to advance the physics simulation by, in seconds.
         */
        virtual void FixedUpdate(float step) = 0;

        /** Performs any physics operations that arent tied to the fixed update interval. Should be called once per frame. */
        virtual void Update() { }

        /** Determines if audio reproduction is paused globally. */
        virtual void SetPaused(bool paused) = 0;

        /** @copydoc SetPaused() */
        virtual bool IsPaused() const = 0;

        /** Checks is the physics simulation update currently in progress. */
        bool IsUpdateInProgress() const { return _updateInProgress; }

    protected:
        bool _updateInProgress = false;
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

    protected:
        PhysicsScene() = default;
        virtual ~PhysicsScene() = default;

    };

    /** Provides easier access to Physics. */
    TE_CORE_EXPORT Physics& gPhysics();
}
