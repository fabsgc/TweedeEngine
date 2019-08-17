#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeVector3.h"

namespace te
{
     /**
      * Represents a rectangle in three dimensional space. It is represented by two axes that extend from the specified
      * origin. Axes should be perpendicular to each other and they extend in both positive and negative directions from the
      * origin by the amount specified by extents.
      */
    class TE_UTILITY_EXPORT Rect3
    {
    public:
        Rect3() = default;

        Rect3(const Vector3& center, const std::array<Vector3, 2>& axes,
            const std::array<float, 2>& extents)
            :_center(center), _axisHorz(axes[0]), _axisVert(axes[1]),
            _extentHorz(extents[0]), _extentVert(extents[1])
        { }

        /**
         * Find the nearest points of the provided ray and the rectangle.
         *
         * @return	A set of nearest points and nearest distance. First value in the set corresponds to nearest point on
         *			the ray, and the second to the nearest point on the rectangle. They are same in the case of intersection.
         *			When ray is parallel to the rectangle there are two sets of nearest points but only one the set nearest
         *			to the ray origin is returned.
         */
        std::pair<std::array<Vector3, 2>, float> GetNearestPoint(const Ray& ray) const;

        /**
         * Find the nearest point on the rectangle to the provided point.
         *
         * @return	Nearest point and distance to nearest point.
         */
        std::pair<Vector3, float> GetNearestPoint(const Vector3& point) const;

        /**
         * Ray/rectangle intersection.
         *
         * @return	Boolean result and distance to intersection point.
         */
        std::pair<bool, float> Intersects(const Ray& ray) const;

        /** Gets the origin of the rectangle. */
        const Vector3& GetCenter() const { return _center; }

        /** Returns the rectangle's horizontal axis. */
        const Vector3& GetAxisHorz() const { return _axisHorz; }

        /** Returns the rectangle's vertical axis. */
        const Vector3& GetAxisVert() const { return _axisVert; }

        /** Gets the extent of the rectangle along its horizontal axis. */
        const float& GetExtentHorz() const { return _extentHorz; }

        /** Gets the extent of the rectangle along its vertical axis. */
        const float& GetExtentVertical() const { return _extentVert; }

    private:
        Vector3 _center { TeZero };
        Vector3 _axisHorz { TeZero };
        Vector3 _axisVert { TeZero };
        float _extentHorz = 0.0f;
        float _extentVert = 0.0f;
    };

    /** @} */
}
