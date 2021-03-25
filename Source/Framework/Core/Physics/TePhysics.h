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
    };

    /** Provides global physics settings, factory methods for physics objects and scene queries. */
    class TE_CORE_EXPORT Physics : public Module<Physics>
    {
    public:
        Physics(const PHYSICS_INIT_DESC& init);
        virtual ~Physics() = default;

        /** Performs any physics operations that arent tied to the fixed update interval. Should be called once per frame. */
        virtual void Update() { }

        /** Determines if audio reproduction is paused globally. */
        virtual void SetPaused(bool paused) = 0;

        /** @copydoc SetPaused() */
        virtual bool IsPaused() const = 0;
    };

    /**
     * Physical representation of a scene, allowing creation of new physical objects in the scene and queries against
     * those objects. Objects created in different scenes cannot physically interact with eachother.
     */
    class TE_CORE_EXPORT PhysicsScene
    {
    public:
        // TODO physics

    protected:
        PhysicsScene() = default;
        virtual ~PhysicsScene() = default;

    };

    /** Provides easier access to Physics. */
    TE_CORE_EXPORT Physics& gPhysics();
}
