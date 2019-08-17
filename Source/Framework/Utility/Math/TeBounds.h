#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeVector3.h"
#include "Math/TeAABox.h"
#include "Math/TeSphere.h"
#include "Math/TeMatrix4.h"

namespace te
{
     /** Bounds represented by an axis aligned box and a sphere. */
    class TE_UTILITY_EXPORT Bounds
    {
    public:
        Bounds() = default;
        Bounds(const AABox& box, const Sphere& sphere);
        ~Bounds() = default;

        /** Returns the axis aligned box representing the bounds. */
        const AABox& GetBox() const { return _box; }

        /** Returns the sphere representing the bounds. */
        const Sphere& GetSphere() const { return _sphere; }

        /** Updates the bounds by setting the new bounding box and sphere. */
        void SetBounds(const AABox& box, const Sphere& sphere);

        /** Merges the two bounds, creating a new bounds that encapsulates them both. */
        void Merge(const Bounds& rhs);

        /** Expands the bounds so it includes the provided point. */
        void Merge(const Vector3& point);

        /**
         * Transforms the bounds by the given matrix.
         *
         * @note
         * As the resulting box will no longer be axis aligned, an axis align box
         * is instead created by encompassing the transformed oriented bounding box.
         * Retrieving the value as an actual OBB would provide a tighter fit.
         */
        void Transform(const Matrix4& matrix);

        /**
         * Transforms the bounds by the given matrix.
         *
         * @note
         * As the resulting box will no longer be axis aligned, an axis align box
         * is instead created by encompassing the transformed oriented bounding box.
         * Retrieving the value as an actual OBB would provide a tighter fit.
         *
         * @note
         * Provided matrix must be affine.
         */
        void TransformAffine(const Matrix4& matrix);

    protected:
        AABox _box;
        Sphere _sphere;
    };
}
