#include "Math/TeSphere.h"
#include "Math/TeRay.h"
#include "Math/TePlane.h"
#include "Math/TeAABox.h"
#include "Math/TeMath.h"
#include "Math/TeMatrix4.h"

namespace te
{
    void Sphere::Merge(const Sphere& rhs)
    {
        Vector3 newCenter = (_center + rhs._center) * 0.5f;

        float newRadiusA = newCenter.Distance(_center) + GetRadius();
        float newRadiusB = newCenter.Distance(rhs._center) + rhs.GetRadius();

        _center = newCenter;
        _radius = std::max(newRadiusA, newRadiusB);
    }

    void Sphere::Merge(const Vector3& point)
    {
        float dist = point.Distance(_center);
        _radius = std::max(_radius, dist);
    }

    void Sphere::Transform(const Matrix4& matrix)
    {
        float lengthSqrd[3];
        for (UINT32 i = 0; i < 3; i++)
        {
            Vector3 column = matrix.GetColumn(i);
            lengthSqrd[i] = column.Dot(column);
        }

        float maxLengthSqrd = std::max(lengthSqrd[0], std::max(lengthSqrd[1], lengthSqrd[2]));

        _center = matrix.MultiplyAffine(_center);
        _radius *= sqrt(maxLengthSqrd);
    }

    bool Sphere::Contains(const Vector3& v) const
    {
        return ((v - _center).SquaredLength() <= Math::Sqr(_radius));
    }

    bool Sphere::Intersects(const Sphere& s) const
    {
        return (s._center - _center).SquaredLength() <=
            Math::Sqr(s._radius + _radius);
    }

    std::pair<bool, float> Sphere::Intersects(const Ray& ray, bool discardInside) const
    {
        const Vector3& raydir = ray.GetDirection();
        const Vector3& rayorig = ray.GetOrigin() - GetCenter();
        float radius = GetRadius();

        // Check origin inside first
        if (rayorig.SquaredLength() <= radius * radius && discardInside)
        {
            return std::pair<bool, float>(true, 0.0f);
        }

        // t = (-b +/- sqrt(b*b + 4ac)) / 2a
        float a = raydir.Dot(raydir);
        float b = 2 * rayorig.Dot(raydir);
        float c = rayorig.Dot(rayorig) - radius * radius;

        // Determinant
        float d = (b*b) - (4 * a * c);
        if (d < 0)
        {
            // No intersection
            return std::pair<bool, float>(false, 0.0f);
        }
        else
        {
            // If d == 0 there is one intersection, if d > 0 there are 2.
            // We only return the first one.

            float t = (-b - Math::Sqrt(d)) / (2 * a);
            if (t < 0)
                t = (-b + Math::Sqrt(d)) / (2 * a);

            return std::pair<bool, float>(true, t);
        }
    }

    bool Sphere::Intersects(const Plane& plane) const
    {
        return (Math::Abs(plane.GetDistance(GetCenter())) <= GetRadius());
    }

    bool Sphere::Intersects(const AABox& box) const
    {
        return box.Intersects(*this);
    }
}