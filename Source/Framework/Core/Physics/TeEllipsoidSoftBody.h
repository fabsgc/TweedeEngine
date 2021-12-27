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

        /**
         * Sets ellipsoid center
         */
        virtual void SetCenter(const Vector3& center) = 0;

        /**
         * Gets ellipsoid center
         */
        virtual Vector3 GetCenter() const = 0;

        /**
         * Sets ellipsoid radius
         */
        virtual void SetRadius(const Vector3& radius) = 0;

        /**
         * Gets ellipsoid radius
         */
        virtual Vector3 GetRadius() const = 0;

        /**
         * Sets ellipsoid resolution
         */
        virtual void SetResolution(UINT32 resolution) = 0;

        /**
         * Gets ellipsoid resolution
         */
        virtual UINT32 GetResolution() const = 0;

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
