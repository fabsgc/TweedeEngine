#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeVector3.h"

namespace te
{
     /** A plane represented by a normal and a distance. */
    class TE_UTILITY_EXPORT Plane
    {
    public:
        /**
         * The "positive side" of the plane is the half space to which the plane normal points. The "negative side" is the
         * other half space. The flag "no side" indicates the plane itself.
         */
        enum Side
        {
            NO_SIDE,
            POSITIVE_SIDE,
            NEGATIVE_SIDE,
            BOTH_SIDE
        };

    public:
        Plane() = default;
        Plane(const Plane& copy) = default;
        Plane(const Vector3& normal, float d);
        Plane(float a, float b, float c, float d);
        Plane(const Vector3& normal, const Vector3& point);
        Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2);

        Plane& operator= (const Plane& rhs) = default;

        /**
         * Returns the side of the plane where the point is located on.
         *
         * @note	NO_SIDE signifies the point is on the plane.
         */
        Side GetSide(const Vector3& point, float epsilon = 0.0f) const;

        /**
         * Returns the side where the alignedBox is. The flag BOTH_SIDE indicates an intersecting box.
         * One corner ON the plane is sufficient to consider the box and the plane intersecting.
         */
        Side GetSide(const AABox& box) const;

        /** Returns the side where the sphere is. The flag BOTH_SIDE indicates an intersecting sphere. */
        Side GetSide(const Sphere& sphere) const;

        /**
         * Returns a distance from point to plane.
         *
         * @note	The sign of the return value is positive if the point is on the
         * 			positive side of the plane, negative if the point is on the negative
         * 			side, and zero if the point is on the plane.
         */
        float GetDistance(const Vector3& point) const;

        /** Project a vector onto the plane. */
        Vector3 ProjectVector(const Vector3& v) const;

        /** Normalizes the plane's normal and the length scale of d. */
        float Normalize();

        /** Box/plane intersection. */
        bool Intersects(const AABox& box) const;

        /** Sphere/plane intersection. */
        bool Intersects(const Sphere& sphere) const;

        /** Ray/plane intersection, returns boolean result and distance to intersection point. */
        std::pair<bool, float> Intersects(const Ray& ray) const;

        bool operator==(const Plane& rhs) const
        {
            return (rhs.d == d && rhs.normal == normal);
        }
        bool operator!=(const Plane& rhs) const
        {
            return (rhs.d != d || rhs.normal != normal);
        }

    public:
        Vector3 normal{ TeZero };
        float d = 0.0f;
    };
}
