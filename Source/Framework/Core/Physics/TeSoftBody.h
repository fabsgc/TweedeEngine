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
        virtual ~SoftBody() = default;

        /**
         * Sets a mesh that represents the softbody geometry. This can be a generic triangle mesh, or and convex mesh.
         */
        virtual void SetMesh(const HPhysicsMesh& mesh) { _mesh = mesh; }

        /** @copydoc SetMesh */
        HPhysicsMesh GetMesh() const { return _mesh; }

        /**
         * We can scale the PhysicsMesh in order to match to the 3D mesh geometry
         */
        virtual void SetScale(const Vector3& scale) { _scale = scale; }

        /** @copydoc SetScale */
        const Vector3& GetScale() const { return _scale; }

        /**
         * Creates a new softbody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        static SPtr<SoftBody> Create(const HSceneObject& linkedSO);

    protected:
        /**
         * Constructs a new softbody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        SoftBody(const HSceneObject& linkedSO);

    protected:
        HPhysicsMesh _mesh;
        Vector3 _scale = Vector3::ONE;
    };
}
