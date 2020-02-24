#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeVector3.h"

namespace te
{
     /** A sphere represented by a center point and a radius. */
    class TE_UTILITY_EXPORT Sphere
    {
    public:
        /** Default constructor. Creates a unit sphere around the origin. */
        Sphere() = default;

        Sphere(const Vector3& center, float radius)
            : _radius(radius), _center(center)
        { }

        /** Returns the radius of the sphere. */
        float GetRadius() const { return _radius; }

        /** Sets the radius of the sphere. */
        void SetRadius(float radius) { _radius = radius; }

        /** Returns the center point of the sphere. */
        const Vector3& GetCenter() const { return _center; }

        /** Sets the center point of the sphere. */
        void SetCenter(const Vector3& center) { _center = center; }

        /** Merges the two spheres, creating a new sphere that encapsulates them both. */
        void Merge(const Sphere& rhs);

        /** Expands the sphere so it includes the provided point. */
        void Merge(const Vector3& point);

        /** Transforms the sphere by the given matrix. */
        void Transform(const Matrix4& matrix);

        /** Returns whether or not this sphere contains the provided point. */
        inline bool Contains(const Vector3& v) const;

        /** Returns whether or not this sphere intersects another sphere. */
        bool Intersects(const Sphere& s) const;

        /** Returns whether or not this sphere intersects a box. */
        bool Intersects(const AABox& box) const;

        /** Returns whether or not this sphere intersects a plane. */
        bool Intersects(const Plane& plane) const;

        /**
         * Ray/sphere intersection, returns boolean result and distance to nearest intersection.
         *
         * @param[in]	ray				Ray to intersect with the sphere.
         * @param[in]	discardInside	(optional) If true the intersection will be discarded if ray origin
         * 								is located within the sphere.
         */
        std::pair<bool, float> Intersects(const Ray& ray, bool discardInside = true) const;

    private:
        float _radius = 1.0f;
        Vector3 _center { Vector3::ZERO };
    };
}
