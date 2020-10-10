#include "Math/TeConvexVolume.h"
#include "Math/TeAABox.h"
#include "Math/TeSphere.h"
#include "Math/TePlane.h"
#include "Math/TeMath.h"

namespace te
{
    ConvexVolume::ConvexVolume(const Vector<Plane>& planes)
        : _planes(planes)
    { }

    ConvexVolume::ConvexVolume(const Matrix4& projectionMatrix, bool useNearPlane)
    {
        _planes.reserve(6);

        const Matrix4& proj = projectionMatrix;

        // Left
        {
            Plane plane;
            plane.normal.x = proj[3][0] + proj[0][0];
            plane.normal.y = proj[3][1] + proj[0][1];
            plane.normal.z = proj[3][2] + proj[0][2];
            plane.d = proj[3][3] + proj[0][3];

            _planes.push_back(plane);
        }

        // Right
        {
            Plane plane;
            plane.normal.x = proj[3][0] - proj[0][0];
            plane.normal.y = proj[3][1] - proj[0][1];
            plane.normal.z = proj[3][2] - proj[0][2];
            plane.d = proj[3][3] - proj[0][3];

            _planes.push_back(plane);
        }

        // Top
        {
            Plane plane;
            plane.normal.x = proj[3][0] - proj[1][0];
            plane.normal.y = proj[3][1] - proj[1][1];
            plane.normal.z = proj[3][2] - proj[1][2];
            plane.d = proj[3][3] - proj[1][3];

            _planes.push_back(plane);
        }

        // Bottom
        {
            Plane plane;
            plane.normal.x = proj[3][0] + proj[1][0];
            plane.normal.y = proj[3][1] + proj[1][1];
            plane.normal.z = proj[3][2] + proj[1][2];
            plane.d = proj[3][3] + proj[1][3];

            _planes.push_back(plane);
        }

        // Far
        {
            Plane plane;
            plane.normal.x = proj[3][0] - proj[2][0];
            plane.normal.y = proj[3][1] - proj[2][1];
            plane.normal.z = proj[3][2] - proj[2][2];
            plane.d = proj[3][3] - proj[2][3];

            _planes.push_back(plane);
        }

        // Near
        if (useNearPlane)
        {
            Plane plane;
            plane.normal.x = proj[3][0] + proj[2][0];
            plane.normal.y = proj[3][1] + proj[2][1];
            plane.normal.z = proj[3][2] + proj[2][2];
            plane.d = proj[3][3] + proj[2][3];

            _planes.push_back(plane);
        }

        for (UINT32 i = 0; i < (UINT32)_planes.size(); i++)
        {
            float length = _planes[i].normal.Normalize();
            _planes[i].d /= -length;
        }
    }

    bool ConvexVolume::Intersects(const AABox& box) const
    {
        Vector3 center = box.GetCenter();
        Vector3 extents = box.GetHalfSize();
        Vector3 absExtents(Math::Abs(extents.x), Math::Abs(extents.y), Math::Abs(extents.z));

        for (auto& plane : _planes)
        {
            float dist = center.Dot(plane.normal) - plane.d;

            float effectiveRadius = absExtents.x * Math::Abs(plane.normal.x);
            effectiveRadius += absExtents.y * Math::Abs(plane.normal.y);
            effectiveRadius += absExtents.z * Math::Abs(plane.normal.z);

            if (dist < -effectiveRadius)
                return false;
        }

        return true;
    }

    bool ConvexVolume::Intersects(const Sphere& sphere) const
    {
        Vector3 center = sphere.GetCenter();
        float radius = sphere.GetRadius();

        for (auto& plane : _planes)
        {
            float dist = center.Dot(plane.normal) - plane.d;

            if (dist < -radius)
                return false;
        }

        return true;
    }

    bool ConvexVolume::Contains(const Vector3& p, float expand) const
    {
        for (auto& plane : _planes)
        {
            if (plane.GetDistance(p) < -expand)
                return false;
        }

        return true;
    }

    const Plane& ConvexVolume::GetPlane(FrustumPlane whichPlane) const
    {
        if (whichPlane >= _planes.size())
        {
            TE_ASSERT_ERROR(false, "Requested plane does not exist in this volume.");
        }

        return _planes[whichPlane];
    }
}
