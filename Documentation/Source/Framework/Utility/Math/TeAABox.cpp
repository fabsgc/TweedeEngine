#include "Math/TeAABox.h"
#include "Math/TeRay.h"
#include "Math/TePlane.h"
#include "Math/TeSphere.h"
#include "Math/TeMath.h"

namespace te
{
    const AABox AABox::BOX_EMPTY = AABox(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
    const AABox AABox::UNIT_BOX = AABox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
    const AABox AABox::INF_BOX = AABox(
        Vector3(
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity()),
        Vector3(
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity()));

    const UINT32 AABox::CUBE_INDICES[36] =
    {
        // Near
        NEAR_LEFT_BOTTOM, NEAR_LEFT_TOP, NEAR_RIGHT_TOP,
        NEAR_LEFT_BOTTOM, NEAR_RIGHT_TOP, NEAR_RIGHT_BOTTOM,

        // Far
        FAR_RIGHT_BOTTOM, FAR_RIGHT_TOP, FAR_LEFT_TOP,
        FAR_RIGHT_BOTTOM, FAR_LEFT_TOP, FAR_LEFT_BOTTOM,

        // Left
        FAR_LEFT_BOTTOM, FAR_LEFT_TOP, NEAR_LEFT_TOP,
        FAR_LEFT_BOTTOM, NEAR_LEFT_TOP, NEAR_LEFT_BOTTOM,

        // Right
        NEAR_RIGHT_BOTTOM, NEAR_RIGHT_TOP, FAR_RIGHT_TOP,
        NEAR_RIGHT_BOTTOM, FAR_RIGHT_TOP, FAR_RIGHT_BOTTOM,

        // Top
        FAR_LEFT_TOP, FAR_RIGHT_TOP, NEAR_RIGHT_TOP,
        FAR_LEFT_TOP, NEAR_RIGHT_TOP, NEAR_LEFT_TOP,

        // Bottom
        NEAR_LEFT_BOTTOM, NEAR_RIGHT_BOTTOM, FAR_RIGHT_BOTTOM,
        NEAR_LEFT_BOTTOM, FAR_RIGHT_BOTTOM, FAR_LEFT_BOTTOM
    };

    AABox::AABox()
    {
        // Default to a unit box
        SetMin(Vector3(-0.5f, -0.5f, -0.5f));
        SetMax(Vector3(0.5f, 0.5f, 0.5f));
    }

    AABox::AABox(const Vector3& min, const Vector3& max)
    {
        SetExtents(min, max);
    }

    void AABox::SetExtents(const Vector3& min, const Vector3& max)
    {
        _minimum = min;
        _maximum = max;
    }

    void AABox::Scale(const Vector3& s)
    {
        Vector3 center = GetCenter();
        Vector3 min = center + (_minimum - center) * s;
        Vector3 max = center + (_maximum - center) * s;

        SetExtents(min, max);
    }

    Vector3 AABox::GetCorner(Corner cornerToGet) const
    {
        switch (cornerToGet)
        {
        case FAR_LEFT_BOTTOM:
            return _minimum;
        case FAR_LEFT_TOP:
            return Vector3(_minimum.x, _maximum.y, _minimum.z);
        case FAR_RIGHT_TOP:
            return Vector3(_maximum.x, _maximum.y, _minimum.z);
        case FAR_RIGHT_BOTTOM:
            return Vector3(_maximum.x, _minimum.y, _minimum.z);
        case NEAR_RIGHT_BOTTOM:
            return Vector3(_maximum.x, _minimum.y, _maximum.z);
        case NEAR_LEFT_BOTTOM:
            return Vector3(_minimum.x, _minimum.y, _maximum.z);
        case NEAR_LEFT_TOP:
            return Vector3(_minimum.x, _maximum.y, _maximum.z);
        case NEAR_RIGHT_TOP:
            return _maximum;
        default:
            return Vector3(TeZero);
        }
    }

    void AABox::Merge(const AABox& rhs)
    {
        Vector3 min = _minimum;
        Vector3 max = _maximum;
        max.Max(rhs._maximum);
        min.Min(rhs._minimum);

        SetExtents(min, max);
    }

    void AABox::Merge(const Vector3& point)
    {
        _maximum.Max(point);
        _minimum.Min(point);
    }

    void AABox::Transform(const Matrix4& matrix)
    {
        // Getting the old values so that we can use the existing merge method.
        Vector3 oldMin = _minimum;
        Vector3 oldMax = _maximum;

        Vector3 currentCorner;
        // We sequentially compute the corners in the following order :
        // 0, 6, 5, 1, 2, 4, 7, 3
        // This sequence allows us to only change one member at a time to get at all corners.

        // For each one, we transform it using the matrix
        // Which gives the resulting point and merge the resulting point.

        // First corner
        // min min min
        currentCorner = oldMin;
        Merge(matrix.MultiplyAffine(currentCorner));

        // min,min,max
        currentCorner.z = oldMax.z;
        Merge(matrix.MultiplyAffine(currentCorner));

        // min max max
        currentCorner.y = oldMax.y;
        Merge(matrix.MultiplyAffine(currentCorner));

        // min max min
        currentCorner.z = oldMin.z;
        Merge(matrix.MultiplyAffine(currentCorner));

        // max max min
        currentCorner.x = oldMax.x;
        Merge(matrix.MultiplyAffine(currentCorner));

        // max max max
        currentCorner.z = oldMax.z;
        Merge(matrix.MultiplyAffine(currentCorner));

        // max min max
        currentCorner.y = oldMin.y;
        Merge(matrix.MultiplyAffine(currentCorner));

        // max min min
        currentCorner.z = oldMin.z;
        Merge(matrix.MultiplyAffine(currentCorner));
    }

    void AABox::TransformAffine(const Matrix4& m)
    {
        Vector3 min = m.GetTranslation();
        Vector3 max = m.GetTranslation();
        for (UINT32 i = 0; i < 3; i++)
        {
            for (UINT32 j = 0; j < 3; j++)
            {
                float e = m[i][j] * _minimum[j];
                float f = m[i][j] * _maximum[j];

                if (e < f)
                {
                    min[i] += e;
                    max[i] += f;
                }
                else
                {
                    min[i] += f;
                    max[i] += e;
                }
            }

        }

        SetExtents(min, max);
    }

    bool AABox::Intersects(const AABox& b2) const
    {
        // Use up to 6 separating planes
        if (_maximum.x < b2._minimum.x)
            return false;
        if (_maximum.y < b2._minimum.y)
            return false;
        if (_maximum.z < b2._minimum.z)
            return false;

        if (_minimum.x > b2._maximum.x)
            return false;
        if (_minimum.y > b2._maximum.y)
            return false;
        if (_minimum.z > b2._maximum.z)
            return false;

        // Otherwise, must be intersecting
        return true;
    }

    bool AABox::Intersects(const Sphere& sphere) const
    {
        // Use splitting planes
        const Vector3& center = sphere.GetCenter();
        float radius = sphere.GetRadius();
        const Vector3& min = GetMin();
        const Vector3& max = GetMax();

        // Arvo's algorithm
        float s, d = 0;
        for (int i = 0; i < 3; ++i)
        {
            if (center[i] < min[i])
            {
                s = center[i] - min[i];
                d += s * s;
            }
            else if (center[i] > max[i])
            {
                s = center[i] - max[i];
                d += s * s;
            }
        }
        return d <= radius * radius;
    }

    bool AABox::Intersects(const Plane& p) const
    {
        return (p.GetSide(*this) == Plane::BOTH_SIDE);
    }

    std::pair<bool, float> AABox::Intersects(const Ray& ray) const
    {
        float lowt = 0.0f;
        float t;
        bool hit = false;
        Vector3 hitpoint(TeZero);
        const Vector3& min = GetMin();
        const Vector3& max = GetMax();
        const Vector3& rayorig = ray.GetOrigin();
        const Vector3& raydir = ray.GetDirection();

        // Check origin inside first
        if ((rayorig.x > min.x && rayorig.y > min.y && rayorig.z > min.z) && (rayorig.x < max.x && rayorig.y < max.y && rayorig.z < max.z))
        {
            return std::pair<bool, float>(true, 0.0f);
        }

        // Check each face in turn, only check closest 3
        // Min x
        if (rayorig.x <= min.x && raydir.x > 0)
        {
            t = (min.x - rayorig.x) / raydir.x;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max x
        if (rayorig.x >= max.x && raydir.x < 0)
        {
            t = (max.x - rayorig.x) / raydir.x;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min y
        if (rayorig.y <= min.y && raydir.y > 0)
        {
            t = (min.y - rayorig.y) / raydir.y;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max y
        if (rayorig.y >= max.y && raydir.y < 0)
        {
            t = (max.y - rayorig.y) / raydir.y;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min z
        if (rayorig.z <= min.z && raydir.z > 0)
        {
            t = (min.z - rayorig.z) / raydir.z;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max z
        if (rayorig.z >= max.z && raydir.z < 0)
        {
            t = (max.z - rayorig.z) / raydir.z;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }

        return std::pair<bool, float>(hit, lowt);

    }

    bool AABox::Intersects(const Ray& ray, float& d1, float& d2) const
    {
        const Vector3& min = GetMin();
        const Vector3& max = GetMax();
        const Vector3& rayorig = ray.GetOrigin();
        const Vector3& raydir = ray.GetDirection();

        Vector3 absDir;
        absDir[0] = Math::Abs(raydir[0]);
        absDir[1] = Math::Abs(raydir[1]);
        absDir[2] = Math::Abs(raydir[2]);

        // Sort the axis, ensure check minimise floating error axis first
        int imax = 0, imid = 1, imin = 2;
        if (absDir[0] < absDir[2])
        {
            imax = 2;
            imin = 0;
        }
        if (absDir[1] < absDir[imin])
        {
            imid = imin;
            imin = 1;
        }
        else if (absDir[1] > absDir[imax])
        {
            imid = imax;
            imax = 1;
        }

        float start = 0, end = Math::POS_INFINITY;

#define _CALC_AXIS(i)                                       \
    do {                                                    \
    float denom = 1 / raydir[i];                         \
    float newstart = (min[i] - rayorig[i]) * denom;      \
    float newend = (max[i] - rayorig[i]) * denom;        \
    if (newstart > newend) std::swap(newstart, newend); \
    if (newstart > end || newend < start) return false; \
    if (newstart > start) start = newstart;             \
    if (newend < end) end = newend;                     \
    } while(0)

        // Check each axis in turn

        _CALC_AXIS(imax);

        if (absDir[imid] < std::numeric_limits<float>::epsilon())
        {
            // Parallel with middle and minimise axis, check bounds only
            if (rayorig[imid] < min[imid] || rayorig[imid] > max[imid] ||
                rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
                return false;
        }
        else
        {
            _CALC_AXIS(imid);

            if (absDir[imin] < std::numeric_limits<float>::epsilon())
            {
                // Parallel with minimise axis, check bounds only
                if (rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
                    return false;
            }
            else
            {
                _CALC_AXIS(imin);
            }
        }
#undef _CALC_AXIS

        d1 = start;
        d2 = end;

        return true;
    }

    Vector3 AABox::GetCenter() const
    {
        return Vector3(
            (_maximum.x + _minimum.x) * 0.5f,
            (_maximum.y + _minimum.y) * 0.5f,
            (_maximum.z + _minimum.z) * 0.5f);
    }

    Vector3 AABox::GetSize() const
    {
        return _maximum - _minimum;
    }

    Vector3 AABox::GetHalfSize() const
    {
        return (_maximum - _minimum) * 0.5;
    }

    float AABox::GetRadius() const
    {
        return ((_maximum - _minimum) * 0.5).Length();
    }

    float AABox::GetVolume() const
    {
        Vector3 diff = _maximum - _minimum;
        return diff.x * diff.y * diff.z;
    }

    bool AABox::Contains(const Vector3& v) const
    {
        return _minimum.x <= v.x && v.x <= _maximum.x &&
            _minimum.y <= v.y && v.y <= _maximum.y &&
            _minimum.z <= v.z && v.z <= _maximum.z;
    }

    bool AABox::Contains(const AABox& other) const
    {
        return this->_minimum.x <= other._minimum.x &&
            this->_minimum.y <= other._minimum.y &&
            this->_minimum.z <= other._minimum.z &&
            other._maximum.x <= this->_maximum.x &&
            other._maximum.y <= this->_maximum.y &&
            other._maximum.z <= this->_maximum.z;
    }

    bool AABox::operator== (const AABox& rhs) const
    {
        return this->_minimum == rhs._minimum &&
            this->_maximum == rhs._maximum;
    }

    bool AABox::operator!= (const AABox& rhs) const
    {
        return !(*this == rhs);
    }
}

