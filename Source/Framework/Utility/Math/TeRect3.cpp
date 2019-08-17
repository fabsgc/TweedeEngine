#include "Math/TeRect3.h"
#include "Math/TeRay.h"
#include "Math/TeLineSegment3.h"

namespace te
{
    std::pair<std::array<Vector3, 2>, float> Rect3::GetNearestPoint(const Ray& ray) const
    {
        const Vector3& org = ray.GetOrigin();
        const Vector3& dir = ray.GetDirection();

        bool foundNearest = false;
        float t = 0.0f;
        std::array<Vector3, 2> nearestPoints{ { Vector3::ZERO, Vector3::ZERO } };
        float distance = 0.0f;

        // Check if Ray intersects the rectangle
        auto intersectResult = Intersects(ray);
        if (intersectResult.first)
        {
            t = intersectResult.second;

            nearestPoints[0] = org + dir * t;
            nearestPoints[1] = nearestPoints[0]; // Just one point of intersection
            foundNearest = true;
        }

        // Ray is either passing next to the rectangle or parallel to it, 
        // compare ray to 4 edges of the rectangle
        if (!foundNearest)
        {
            Vector3 scaledAxes[2];
            scaledAxes[0] = _extentHorz * _axisHorz;
            scaledAxes[1] = _extentVert * _axisVert;;

            distance = std::numeric_limits<float>::max();
            for (UINT32 i = 0; i < 2; i++)
            {
                for (UINT32 j = 0; j < 2; j++)
                {
                    float sign = (float)(2 * j - 1);
                    Vector3 segCenter = _center + sign * scaledAxes[i];
                    Vector3 segStart = segCenter - scaledAxes[1 - i];
                    Vector3 segEnd = segCenter + scaledAxes[1 - i];

                    LineSegment3 segment(segStart, segEnd);
                    auto segResult = segment.GetNearestPoint(ray);

                    if (segResult.second < distance)
                    {
                        nearestPoints = segResult.first;
                        distance = segResult.second;
                    }
                }
            }
        }

        // Front of the ray is nearest, use found points
        if (t >= 0.0f)
        {
            // Do nothing, we already have the points
        }
        else // Rectangle is behind the ray origin, find nearest point to origin
        {
            auto nearestPointToOrg = GetNearestPoint(org);

            nearestPoints[0] = org;
            nearestPoints[1] = nearestPointToOrg.first;
            distance = nearestPointToOrg.second;
        }

        return std::make_pair(nearestPoints, distance);
    }

    std::pair<Vector3, float> Rect3::GetNearestPoint(const Vector3& point) const
    {
        Vector3 diff = _center - point;
        float b0 = diff.Dot(_axisHorz);
        float b1 = diff.Dot(_axisVert);
        float s0 = -b0, s1 = -b1;
        float sqrDistance = diff.Dot(diff);

        if (s0 < -_extentHorz)
            s0 = -_extentHorz;
        else if (s0 > _extentHorz)
            s0 = _extentHorz;

        sqrDistance += s0 * (s0 + 2.0f*b0);

        if (s1 < -_extentVert)
            s1 = -_extentVert;
        else if (s1 > _extentVert)
            s1 = _extentVert;

        sqrDistance += s1 * (s1 + 2.0f*b1);

        if (sqrDistance < 0.0f)
            sqrDistance = 0.0f;

        float dist = std::sqrt(sqrDistance);
        Vector3 nearestPoint = _center + s0 * _axisHorz + s1 * _axisVert;

        return std::make_pair(nearestPoint, dist);
    }

    std::pair<bool, float> Rect3::Intersects(const Ray& ray) const
    {
        const Vector3& org = ray.GetOrigin();
        const Vector3& dir = ray.GetDirection();

        Vector3 normal = _axisHorz.Cross(_axisVert);
        float NdotD = normal.Dot(dir);
        if (fabs(NdotD) > 0.0f)
        {
            Vector3 diff = org - _center;
            Vector3 basis[3];

            basis[0] = dir;
            basis[0].OrthogonalComplement(basis[1], basis[2]);

            float UdD0 = basis[1].Dot(_axisHorz);
            float UdD1 = basis[1].Dot(_axisVert);
            float UdPmC = basis[1].Dot(diff);
            float VdD0 = basis[2].Dot(_axisHorz);
            float VdD1 = basis[2].Dot(_axisVert);
            float VdPmC = basis[2].Dot(diff);
            float invDet = 1.0f / (UdD0*VdD1 - UdD1 * VdD0);

            float s0 = (VdD1*UdPmC - UdD1 * VdPmC)*invDet;
            float s1 = (UdD0*VdPmC - VdD0 * UdPmC)*invDet;

            if (fabs(s0) <= _extentHorz && fabs(s1) <= _extentVert)
            {
                float DdD0 = dir.Dot(_axisHorz);
                float DdD1 = dir.Dot(_axisVert);
                float DdDiff = dir.Dot(diff);

                float t = s0 * DdD0 + s1 * DdD1 - DdDiff;

                return std::make_pair(true, t);
            }
        }

        return std::make_pair(false, 0.0f);
    }
}
