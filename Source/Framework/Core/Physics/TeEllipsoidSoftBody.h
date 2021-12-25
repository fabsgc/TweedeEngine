#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSoftBody.h"

namespace te
{
    /**
     * A SoftBody representing a customizable ellipsoid
     */
    class TE_CORE_EXPORT EllipsoidSoftBody : public SoftBody
    {
    public:
        /**
         * Creates a new mesh softbody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        static SPtr<EllipsoidSoftBody> Create(const HSceneObject& linkedSO);

    protected:
        /**
         * Constructs a new EllipsoidSoftBody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        explicit EllipsoidSoftBody(const HSceneObject& linkedSO);
    };
}
