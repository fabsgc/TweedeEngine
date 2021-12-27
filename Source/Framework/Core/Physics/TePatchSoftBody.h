#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSoftBody.h"

namespace te
{
    /**
     * A SoftBody representing a customizable ellipsoid
     */
    class TE_CORE_EXPORT PatchSoftBody : public SoftBody
    {
    public:
        /**
         * Creates a new mesh softbody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        static SPtr<PatchSoftBody> Create(const HSceneObject& linkedSO);

        /**
         * Sets the 4 patch corners position
         */
        virtual void SetCorners(const Vector3& topLeft, const Vector3& topRight, const Vector3& bottomLeft, const Vector3& bottomRight) = 0;

        /**
         * Sets the 4 patch corners position
         */
        virtual void GetCorners(Vector3& topLeft, Vector3& topRight, Vector3& bottomLeft, Vector3& bottomRight) const = 0;

        /**
         * Sets the x and y patch resolution
         */
        virtual void SetResolution(UINT32 x, UINT32 y) = 0;

        /**
         * Gets the x and y patch resolution
         */
        virtual void GetResolution(UINT32& x, UINT32& y) const = 0;

    protected:
        /**
         * Constructs a new PatchSoftBody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         */
        explicit PatchSoftBody(const HSceneObject& linkedSO);
    };
}
