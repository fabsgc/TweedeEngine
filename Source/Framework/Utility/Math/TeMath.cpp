#include "Math/TeMath.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Math/TeQuaternion.h"

namespace te
{
    const float Math::LOG2 = std::log(2.0f);

    Radian Math::Acos(float val)
    {
        if (-1.0f < val)
        {
            if (val < 1.0f)
                return Radian(std::acos(val));
            else
                return Radian(0.0f);
        }
        else
        {
            return Radian(PI);
        }
    }

    Radian Math::Asin(float val)
    {
        if (-1.0f < val)
        {
            if (val < 1.0f)
                return Radian(std::asin(val));
            else
                return Radian(HALF_PI);
        }
        else
        {
            return Radian(-HALF_PI);
        }
    }

    float Math::Sign(float val)
    {
        if (val > 0.0f)
            return 1.0f;

        if (val < 0.0f)
            return -1.0f;

        return 0.0f;
    }

    float Math::InvSqrt(float val)
    {
        return 1.0f / sqrt(val);
    }

    float Math::FastSin0(float val)
    {
        float angleSqr = val * val;
        float result = 7.61e-03f;
        result *= angleSqr;
        result -= 1.6605e-01f;
        result *= angleSqr;
        result += 1.0f;
        result *= val;
        return result;
    }

    float Math::FastSin1(float val)
    {
        float angleSqr = val * val;
        float result = -2.39e-08f;
        result *= angleSqr;
        result += 2.7526e-06f;
        result *= angleSqr;
        result -= 1.98409e-04f;
        result *= angleSqr;
        result += 8.3333315e-03f;
        result *= angleSqr;
        result -= 1.666666664e-01f;
        result *= angleSqr;
        result += 1.0f;
        result *= val;

        return result;
    }

    float Math::FastCos0(float val)
    {
        float angleSqr = val * val;
        float result = 3.705e-02f;
        result *= angleSqr;
        result -= 4.967e-01f;
        result *= angleSqr;
        result += 1.0f;

        return result;
    }

    float Math::FastCos1(float val)
    {
        float angleSqr = val * val;
        float result = -2.605e-07f;
        result *= angleSqr;
        result += 2.47609e-05f;
        result *= angleSqr;
        result -= 1.3888397e-03f;
        result *= angleSqr;
        result += 4.16666418e-02f;
        result *= angleSqr;
        result -= 4.999999963e-01f;
        result *= angleSqr;
        result += 1.0f;

        return result;
    }

    float Math::FastTan0(float val)
    {
        float angleSqr = val * val;
        float result = 2.033e-01f;
        result *= angleSqr;
        result += 3.1755e-01f;
        result *= angleSqr;
        result += 1.0f;
        result *= val;
        return result;
    }

    float Math::FastTan1(float val)
    {
        float angleSqr = val * val;
        float result = 9.5168091e-03f;
        result *= angleSqr;
        result += 2.900525e-03f;
        result *= angleSqr;
        result += 2.45650893e-02f;
        result *= angleSqr;
        result += 5.33740603e-02f;
        result *= angleSqr;
        result += 1.333923995e-01f;
        result *= angleSqr;
        result += 3.333314036e-01f;
        result *= angleSqr;
        result += 1.0f;
        result *= val;
        return result;
    }

    float Math::FastASin0(float val)
    {
        float root = sqrt(abs(1.0f - val));
        float result = -0.0187293f;
        result *= val;
        result += 0.0742610f;
        result *= val;
        result -= 0.2121144f;
        result *= val;
        result += 1.5707288f;
        result = HALF_PI - root * result;
        return result;
    }

    float Math::FastASin1(float val)
    {
        float root = sqrt(abs(1.0f - val));
        float result = -0.0012624911f;
        result *= val;
        result += 0.0066700901f;
        result *= val;
        result -= 0.0170881256f;
        result *= val;
        result += 0.0308918810f;
        result *= val;
        result -= 0.0501743046f;
        result *= val;
        result += 0.0889789874f;
        result *= val;
        result -= 0.2145988016f;
        result *= val;
        result += 1.5707963050f;
        result = HALF_PI - root * result;
        return result;
    }

    float Math::FastACos0(float val)
    {
        float root = sqrt(abs(1.0f - val));
        float result = -0.0187293f;
        result *= val;
        result += 0.0742610f;
        result *= val;
        result -= 0.2121144f;
        result *= val;
        result += 1.5707288f;
        result *= root;
        return result;
    }

    float Math::FastACos1(float val)
    {
        float root = sqrt(abs(1.0f - val));
        float result = -0.0012624911f;
        result *= val;
        result += 0.0066700901f;
        result *= val;
        result -= 0.0170881256f;
        result *= val;
        result += 0.0308918810f;
        result *= val;
        result -= 0.0501743046f;
        result *= val;
        result += 0.0889789874f;
        result *= val;
        result -= 0.2145988016f;
        result *= val;
        result += 1.5707963050f;
        result *= root;
        return result;
    }

    float Math::FastATan0(float val)
    {
        float valueSqr = val * val;
        float result = 0.0208351f;
        result *= valueSqr;
        result -= 0.085133f;
        result *= valueSqr;
        result += 0.180141f;
        result *= valueSqr;
        result -= 0.3302995f;
        result *= valueSqr;
        result += 0.999866f;
        result *= val;
        return result;
    }

    float Math::FastATan1(float val)
    {
        float valueSqr = val * val;
        float result = 0.0028662257f;
        result *= valueSqr;
        result -= 0.0161657367f;
        result *= valueSqr;
        result += 0.0429096138f;
        result *= valueSqr;
        result -= 0.0752896400f;
        result *= valueSqr;
        result += 0.1065626393f;
        result *= valueSqr;
        result -= 0.1420889944f;
        result *= valueSqr;
        result += 0.1999355085f;
        result *= valueSqr;
        result -= 0.3333314528f;
        result *= valueSqr;
        result += 1.0f;
        result *= val;
        return result;
    }

    bool Math::ApproxEquals(const Vector2& a, const Vector2& b, float tolerance)
    {
        return fabs(b.x - a.x) <= tolerance && fabs(b.y - a.y) <= tolerance;
    }

    bool Math::ApproxEquals(const Vector3& a, const Vector3& b, float tolerance)
    {
        return fabs(b.x - a.x) <= tolerance && fabs(b.y - a.y) <= tolerance && fabs(b.z - a.z) <= tolerance;
    }

    bool Math::ApproxEquals(const Vector4& a, const Vector4& b, float tolerance)
    {
        return fabs(b.x - a.x) <= tolerance && fabs(b.y - a.y) <= tolerance && fabs(b.z - a.z) <= tolerance &&
            fabs(b.w - a.w) <= tolerance;
    }

    bool Math::ApproxEquals(const Quaternion& a, const Quaternion& b, float tolerance)
    {
        return fabs(b.x - a.x) <= tolerance && fabs(b.y - a.y) <= tolerance && fabs(b.z - a.z) <= tolerance &&
            fabs(b.w - a.w) <= tolerance;
    }

    Vector3 Math::CalculateTriTangent(const Vector3& position1, const Vector3& position2,
        const Vector3& position3, float u1, float v1, float u2, float v2, float u3, float v3)
    {
        Vector3 side0 = position1 - position2;
        Vector3 side1 = position3 - position1;

        // Calculate face normal
        Vector3 normal = side1.Cross(side0);
        normal.Normalize();

        // Now we use a formula to calculate the tangent. 
        float deltaV0 = v1 - v2;
        float deltaV1 = v3 - v1;
        Vector3 tangent = deltaV1 * side0 - deltaV0 * side1;
        tangent.Normalize();

        // Calculate bitangent
        float deltaU0 = u1 - u2;
        float deltaU1 = u3 - u1;
        Vector3 bitangent = deltaU1 * side0 - deltaU0 * side1;
        bitangent.Normalize();

        // Now, we take the cross product of the tangents to get a vector which 
        // should point in the same direction as our normal calculated above. 
        // If it points in the opposite direction (the dot product between the normals is less than zero), 
        // then we need to reverse the s and t tangents. 
        // This is because the triangle has been mirrored when going from tangent space to object space.
        // reverse tangents if necessary.
        Vector3 tangentCross = tangent.Cross(bitangent);
        if (tangentCross.Dot(normal) < 0.0f)
        {
            tangent = -tangent;
            bitangent = -bitangent;
        }

        return tangent;
    }

    Degree Math::WrapAngle(Degree angle)
    {
        if (angle.ValueDegrees() < -360.0f)
            angle += Degree(360.0f);

        if (angle.ValueDegrees() > 360.0f)
            angle -= Degree(360.0f);

        return angle;
    }
}
