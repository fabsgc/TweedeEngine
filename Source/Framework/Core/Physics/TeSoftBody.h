#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeBody.h"

namespace te
{
    /**
     * SoftBody is a dynamic physics object that can be moved using forces (or directly). It will interact with other
     * static and dynamic physics objects in the scene accordingly (it will push other non-kinematic rigidbodies,
     * and collide with static objects).
     */
    class TE_CORE_EXPORT SoftBody : public Body
    {
    public:

        /**
         * We can scale the PhysicsMesh in order to match to the 3D mesh geometry
         */
        void SetScale(const Vector3& scale);

        /** @copydoc SetScale */
        const Vector3& GetScale() const;

    protected:
        /**
         * Constructs a new SoftBody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        explicit SoftBody(const HSceneObject& linkedSO);
    };
}
