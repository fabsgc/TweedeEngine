#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSoftBody.h"

namespace te
{
    /**
     * A SoftBody which is build using a PhysicsMesh
     */
    class TE_CORE_EXPORT MeshSoftBody : public SoftBody
    {
    public:

        /**
         * Creates a new mesh softbody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        static SPtr<MeshSoftBody> Create(const HSceneObject& linkedSO);

    protected:
        /**
         * Constructs a new softbody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        MeshSoftBody(const HSceneObject& linkedSO);
    };
}
