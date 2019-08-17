#include "Math/TeBounds.h"
#include "Math/TeRay.h"
#include "Math/TePlane.h"
#include "Math/TeSphere.h"

namespace te
{
    Bounds::Bounds(const AABox& box, const Sphere& sphere)
        :_box(box), _sphere(sphere)
    { }

    void Bounds::SetBounds(const AABox& box, const Sphere& sphere)
    {
        _box = box;
        _sphere = sphere;
    }

    void Bounds::Merge(const Bounds& rhs)
    {
        _box.Merge(rhs._box);
        _sphere.Merge(rhs._sphere);
    }

    void Bounds::Merge(const Vector3& point)
    {
        _box.Merge(point);
        _sphere.Merge(point);
    }

    void Bounds::Transform(const Matrix4& matrix)
    {
        _box.Transform(matrix);
        _sphere.Transform(matrix);
    }

    void Bounds::TransformAffine(const Matrix4& matrix)
    {
        _box.TransformAffine(matrix);
        _sphere.Transform(matrix);
    }
}

