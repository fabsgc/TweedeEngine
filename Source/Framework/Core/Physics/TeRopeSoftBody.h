#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSoftBody.h"

namespace te
{
    /**
     * A SoftBody representing a customizable rope
     */
    class TE_CORE_EXPORT RopeSoftBody : public SoftBody
    {
    public:
        /**
         * Creates a new mesh softbody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        static SPtr<RopeSoftBody> Create(const HSceneObject& linkedSO);

    protected:
        /**
         * Constructs a new RopeSoftBody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        explicit RopeSoftBody(const HSceneObject& linkedSO);
    };
}
