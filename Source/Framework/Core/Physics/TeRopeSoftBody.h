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

        /**
         * Sets rope origin
         */
        virtual void SetFrom(const Vector3& from) = 0;

        /**
         * Gets rope origin
         */
        virtual Vector3 GetFrom() const = 0;

        /**
         * Sets rope end
         */
        virtual void SetTo(const Vector3& to) = 0;

        /**
         * Gets rope end
         */
        virtual Vector3 GetTo() const = 0;

        /**
         * Sets rope resolution
         */
        virtual void SetResolution(UINT32 resolution) = 0;

        /**
         * Gets rope resolution
         */
        virtual UINT32 GetResolution() const = 0;

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
