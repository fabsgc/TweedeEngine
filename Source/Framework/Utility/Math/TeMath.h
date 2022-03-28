#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeRadian.h"
#include "Math/TeDegree.h"
#include "Math/TeVector3.h"

#include <cmath>

namespace te
{
    class TE_UTILITY_EXPORT Math
    {
    public:
        static constexpr float BIGGEST_FLOAT_SMALLER_THAN_ONE = 0.99999994f;

        /** Inverse cosine. */
        static Radian Acos(float val);

        /** Inverse sine. */
        static Radian Asin(float val);

        /** Inverse tangent. */
        static Radian Atan(float val) { return Radian(std::atan(val)); }

        /** Inverse tangent with two arguments, returns angle between the X axis and the point. */
        static Radian Atan2(float y, float x) { return Radian(std::atan2(y, x)); }

        /** Cosine. */
        static float Cos(const Radian& val) { return (float)std::cos(val.ValueRadians()); }

        /** Cosine. */
        static float Cos(float val) { return (float)std::cos(val); }

        /** Sine. */
        static float Sin(const Radian& val) { return (float)std::sin(val.ValueRadians()); }

        /** Sine. */
        static float Sin(float val) { return (float)std::sin(val); }

        /** Tangent. */
        static float Tan(const Radian& val) { return (float)std::tan(val.ValueRadians()); }

        /** Tangent. */
        static float Tan(float val) { return (float)std::tan(val); }

        /** Square root. */
        static float Sqrt(float val) { return (float)std::sqrt(val); }

        /** Square root. */
        static Radian Sqrt(const Radian& val) { return Radian(std::sqrt(val.ValueRadians())); }

        /** Square root. */
        static Degree Sqrt(const Degree& val) { return Degree(std::sqrt(val.ValueDegrees())); }

        /** Square root followed by an inverse. */
        static float InvSqrt(float val);

        /** Returns square of the provided value. */
        static float Sqr(float val) { return val * val; }

        /** Returns base raised to the provided power. */
        static float Pow(float base, float exponent) { return (float)std::pow(base, exponent); }

        /** Returns euler number (e) raised to the provided power. */
        static float Exp(float val) { return (float)std::exp(val); }

        /** Returns natural (base e) logarithm of the provided value. */
        static float Log(float val) { return (float)std::log(val); }

        /** Returns base 2 logarithm of the provided value. */
        static float Log2(float val) { return (float)(std::log(val) / LOG2); }

        /** Returns base N logarithm of the provided value. */
        static float LogN(float base, float val) { return (float)(std::log(val) / std::log(base)); }

        /** Returns the sign of the provided value as 1 or -1. */
        static float Sign(float val);

        /** Returns the sign of the provided value as 1 or -1. */
        static Radian Sign(const Radian& val) { return Radian(Sign(val.ValueRadians())); }

        /** Returns the sign of the provided value as 1 or -1. */
        static Degree Sign(const Degree& val) { return Degree(Sign(val.ValueDegrees())); }

        /** Returns the absolute value. */
        static float Abs(float val) { return float(std::fabs(val)); }

        /** Returns the absolute value. */
        static Degree Abs(const Degree& val) { return Degree(std::fabs(val.ValueDegrees())); }

        /** Returns the absolute value. */
        static Radian Abs(const Radian& val) { return Radian(std::fabs(val.ValueRadians())); }

        /** Returns the nearest integer equal or higher to the provided value. */
        static float Ceil(float val) { return (float)std::ceil(val); }

        /** Determines if an integer if a power of 2 or not */
        static bool IsPowerOfTo(int32_t val)
        {
            if (val == 0)
                return true;

            return (val & (val - 1)) == 0;
        }

        /** Returns the next power of 2 */
        static int32_t ToNextPowerOf2(int32_t val)
        {
            if (val == 0)
                return 0;

            val |= (val >> 1);
            val |= (val >> 2);
            val |= (val >> 4);
            val |= (val >> 8);
            val |= (val >> 16);
            val++;

            return val;
        }

        /** Returns the previous power of 2 */
        static int32_t ToPreviousPowerOf2(int32_t val)
        {
            if (val == 0)
                return 0;

            val = ToNextPowerOf2(val);
            return val - (val >> 1);
        }

        /**
         * Returns the nearest integer equal or higher to the provided value. If you are sure the input is positive use
         * ceilToPosInt() for a slightly faster operation.
         */
        static int32_t CeilToInt(float val)
        {
            assert(val >= std::numeric_limits<int32_t>::min() && val <= std::numeric_limits<int32_t>::max());

            // Positive values need offset in order to truncate towards positive infinity (cast truncates towards zero) 
            return val >= 0.0f ? (int32_t)(val + BIGGEST_FLOAT_SMALLER_THAN_ONE) : (int32_t)val;
        }

        /**
         * Returns the nearest integer equal or higher to the provided value. Value must be non-negative. Slightly faster
         * than ceilToInt().
         */
        static uint32_t CeilToPosInt(float val)
        {
            assert(val >= 0 && val <= std::numeric_limits<uint32_t>::max());

            return (uint32_t)(val + BIGGEST_FLOAT_SMALLER_THAN_ONE);
        }

        /** Returns the integer nearest to the provided value. */
        static float Round(float val) { return (float)std::floor(val + 0.5f); }

        /**
         * Returns the integer nearest to the provided value. If you are sure the input is positive use roundToPosInt()
         * for a slightly faster operation.
         */
        static int32_t RoundToInt(float val) { return FloorToInt(val + 0.5f); }

        /**
         * Returns the integer nearest to the provided value. Value must be non-negative. Slightly faster than roundToInt().
         */
        static uint32_t RoundToPosInt(float val) { return FloorToPosInt(val + 0.5f); }

        /**
         * Divides an integer by another integer and returns the result, rounded up. Only works if both integers are
         * positive.
         */
        template<class T>
        static constexpr T DivideAndRoundUp(T n, T d) { return (n + d - 1) / d; }

        /** Returns the nearest integer equal or lower of the provided value. */
        static float Floor(float val) { return (float)floor(val); }

        /**
         * Returns the nearest integer equal or lower of the provided value. If you are sure the input is positive
         * use floorToPosInt() for a slightly faster operation.
         */
        static int FloorToInt(float val)
        {
            //assert(val >= std::numeric_limits<int32_t>::min() && val <= std::numeric_limits<int32_t>::max()); very slow check

            // Negative values need offset in order to truncate towards negative infinity (cast truncates towards zero) 
            return val >= 0.0f ? (int32_t)val : (int32_t)(val - BIGGEST_FLOAT_SMALLER_THAN_ONE);
        }

        /**
         * Returns the nearest integer equal or lower of the provided value. Value must be non-negative. Slightly faster
         * than floorToInt().
         */
        static uint32_t FloorToPosInt(float val)
        {
            assert(val >= 0 && val <= std::numeric_limits<uint32_t>::max());

            return (uint32_t)val;
        }

        /** Rounds @p x to the nearest multiple of @p multiple. */
        static float RoundToMultiple(float x, float multiple)
        {
            return Floor((x + multiple * 0.5f) / multiple) * multiple;
        }

        /** Clamp a value within an inclusive range. */
        template <typename T>
        static T Clamp(const T& val, const T& minval, const T& maxval)
        {
            assert(minval <= maxval && "Invalid clamp range");
            return std::max(std::min(val, maxval), minval);
        }

        /** Clamp a value within an inclusive range [0..1]. */
        template <typename T>
        static T Clamp01(const T& val)
        {
            return std::max(std::min(val, (T)1), (T)0);
        }

        /** Returns the fractional part of a floating point number. */
        static float Frac(float val)
        {
            return val - (float)(int32_t)val;
        }

        /** Returns a floating point remainder for (@p val / @p length). */
        static float Repeat(float val, float length)
        {
            return val - Floor(val / length) * length;
        }

        /**
         * Wraps the value in range [0, length) and reverses the direction every @p length increment. This results in
         * @p val incrementing until @p length, then decrementing back to 0, and so on.
         */
        static float PingPong(float val, float length)
        {
            val = Repeat(val, length * 2.0f);
            return length - fabs(val - length);
        }

        /** Checks if the value is a valid number. */
        static bool IsNaN(float f)
        {
            return f != f;
        }

        /** Compare two floats, using tolerance for inaccuracies. */
        static bool ApproxEquals(const float& a, const float& b,
            float tolerance = std::numeric_limits<float>::epsilon())
        {
            return fabs(b - a) <= tolerance;
        }

        /** Compare two doubles, using tolerance for inaccuracies. */
        static bool ApproxEquals(const double& a, const double& b,
            double tolerance = std::numeric_limits<double>::epsilon())
        {
            return fabs(b - a) <= tolerance;
        }

        /** Compare two 2D vectors, using tolerance for inaccuracies. */
        static bool ApproxEquals(const Vector2& a, const Vector2& b,
            float tolerance = std::numeric_limits<float>::epsilon());

        /** Compare two 3D vectors, using tolerance for inaccuracies. */
        static bool ApproxEquals(const Vector3& a, const Vector3& b,
            float tolerance = std::numeric_limits<float>::epsilon());

        /** Compare two 4D vectors, using tolerance for inaccuracies. */
        static bool ApproxEquals(const Vector4& a, const Vector4& b,
            float tolerance = std::numeric_limits<float>::epsilon());

        /** Compare two quaternions, using tolerance for inaccuracies. */
        static bool ApproxEquals(const Quaternion& a, const Quaternion& b,
            float tolerance = std::numeric_limits<float>::epsilon());

        /** Calculates the tangent space vector for a given set of positions / texture coords. */
        static Vector3 CalculateTriTangent(const Vector3& position1, const Vector3& position2,
            const Vector3& position3, float u1, float v1, float u2, float v2, float u3, float v3);

        /************************************************************************/
        /* 							TRIG APPROXIMATIONS                      	*/
        /************************************************************************/

        /**
         * Sine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastSin0(const Radian& val) { return (float)FastASin0(val.ValueRadians()); }

        /**
         * Sine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastSin0(float val);

        /**
         * Sine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastSin0.
         */
        static float FastSin1(const Radian& val) { return (float)FastASin1(val.ValueRadians()); }

        /**
         * Sine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastSin0.
         */
        static float FastSin1(float val);

        /**
         * Cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastCos0(const Radian& val) { return (float)FastACos0(val.ValueRadians()); }

        /**
         * Cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastCos0(float val);

        /**
         * Cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastCos0.
         */
        static float FastCos1(const Radian& val) { return (float)FastACos1(val.ValueRadians()); }

        /**
         * Cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/2].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastCos0.
         */
        static float FastCos1(float val);

        /**
         * Tangent function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/4].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastTan0(const Radian& val) { return (float)FastATan0(val.ValueRadians()); }

        /**
         * Tangent function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/4].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastTan0(float val);

        /**
         * Tangent function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/4].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastTan0.
         */
        static float FastTan1(const Radian& val) { return (float)FastATan1(val.ValueRadians()); }

        /**
         * Tangent function approximation.
         *
         * @param[in]	val	Angle in range [0, pi/4].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastTan0.
         */
        static float FastTan1(float val);

        /**
         * Inverse sine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastASin0(const Radian& val) { return (float)FastASin0(val.ValueRadians()); }

        /**
         * Inverse sine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastASin0(float val);

        /**
         * Inverse sine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastASin0.
         */
        static float FastASin1(const Radian& val) { return (float)FastASin1(val.ValueRadians()); }

        /**
         * Inverse sine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastASin0.
         */
        static float FastASin1(float val);

        /**
         * Inverse cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastACos0(const Radian& val) { return (float)FastACos0(val.ValueRadians()); }

        /**
         * Inverse cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastACos0(float val);

        /**
         * Inverse cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastACos0.
         */
        static float FastACos1(const Radian& val) { return (float)FastACos1(val.ValueRadians()); }

        /**
         * Inverse cosine function approximation.
         *
         * @param[in]	val	Angle in range [0, 1].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastACos0.
         */
        static float FastACos1(float val);

        /**
         * Inverse tangent function approximation.
         *
         * @param[in]	val	Angle in range [-1, 1].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastATan0(const Radian& val) { return (float)FastATan0(val.ValueRadians()); }

        /**
         * Inverse tangent function approximation.
         *
         * @param[in]	val	Angle in range [-1, 1].
         *
         * @note	Evaluates trigonometric functions using polynomial approximations.
         */
        static float FastATan0(float val);

        /**
         * Inverse tangent function approximation.
         *
         * @param[in]	val	Angle in range [-1, 1].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastATan0.
         */
        static float FastATan1(const Radian& val) { return (float)FastATan1(val.ValueRadians()); }

        /**
         * Inverse tangent function approximation.
         *
         * @param[in]	val	Angle in range [-1, 1].
         *
         * @note
         * Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than fastATan0.
         */
        static float FastATan1(float val);

        /**
         * Linearly interpolates between the two values using @p t. t should be in [0, 1] range, where t = 0 corresponds
         * to @p min value, while t = 1 corresponds to @p max value.
         */
        template <typename T>
        static T Lerp(const float& t, const T& min, const T& max)
        {
            return (1.0f - t) * min + t * max;
        }
        /**
         * Determines the position of a value between two other values. Returns 0 if @p value is less or equal than
         * @p min, 1 if @p value is equal or greater than @p max, and value in range (0, 1) otherwise.
         */
        template <typename T>
        static float InvLerp(const T& val, const T& min, const T& max)
        {
            return Clamp01((val - min) / std::max(max - min, 0.0001F));
        }

        /**
         * Solves the linear equation with the parameters A, B. Returns number of roots found and the roots themselves will
         * be output in the @p roots array.
         *
         * @param[in]	A		First variable.
         * @param[in]	B		Second variable.
         * @param[out]	roots	Must be at least size of 1.
         *
         * @note	Only returns real roots.
         */
        template <typename T>
        static UINT32 SolveLinear(T A, T B, T* roots)
        {
            if (!approxEquals(A, (T)0))
            {
                roots[0] = -B / A;
                return 1;
            }

            roots[0] = 0.0f;
            return 1;
        }

        /**
         * Solves the quadratic equation with the parameters A, B, C. Returns number of roots found and the roots themselves
         * will be output in the @p roots array.
         *
         * @param[in]	A		First variable.
         * @param[in]	B		Second variable.
         * @param[in]	C		Third variable.
         * @param[out]	roots	Must be at least size of 2.
         *
         * @note	Only returns real roots.
         */
        template <typename T>
        static UINT32 SolveQuadratic(T A, T B, T C, T* roots)
        {
            if (!approxEquals(A, (T)0))
            {
                T p = B / (2 * A);
                T q = C / A;
                T D = p * p - q;

                if (!approxEquals(D, (T)0))
                {
                    if (D < (T)0)
                        return 0;

                    T sqrtD = sqrt(D);
                    roots[0] = sqrtD - p;
                    roots[1] = -sqrtD - p;

                    return 2;
                }
                else
                {
                    roots[0] = -p;
                    roots[1] = -p;

                    return 1;
                }
            }
            else
            {
                return solveLinear(B, C, roots);
            }
        }

        /**
         * Solves the cubic equation with the parameters A, B, C, D. Returns number of roots found and the roots themselves
         * will be output in the @p roots array.
         *
         * @param[in]	A		First variable.
         * @param[in]	B		Second variable.
         * @param[in]	C		Third variable.
         * @param[in]	D		Fourth variable.
         * @param[out]	roots	Must be at least size of 3.
         *
         * @note	Only returns real roots.
         */
        template <typename T>
        static UINT32 SolveCubic(T A, T B, T C, T D, T* roots)
        {
            static const T THIRD = (1 / (T)3);

            T invA = 1 / A;
            A = B * invA;
            B = C * invA;
            C = D * invA;

            T sqA = A * A;
            T p = THIRD * (-THIRD * sqA + B);
            T q = ((T)0.5) * ((2 / (T)27) * A * sqA - THIRD * A * B + C);

            T cbp = p * p * p;
            D = q * q + cbp;

            UINT32 numRoots = 0;
            if (!ApproxEquals(D, (T)0))
            {
                if (D < 0.0)
                {
                    T phi = THIRD * ::acos(-q / sqrt(-cbp));
                    T t = 2 * sqrt(-p);

                    roots[0] = t * cos(phi);
                    roots[1] = -t * cos(phi + PI * THIRD);
                    roots[2] = -t * cos(phi - PI * THIRD);

                    numRoots = 3;
                }
                else
                {
                    T sqrtD = sqrt(D);
                    T u = cbrt(sqrtD + fabs(q));

                    if (q > (T)0)
                        roots[0] = -u + p / u;
                    else
                        roots[0] = u - p / u;

                    numRoots = 1;
                }
            }
            else
            {
                if (!ApproxEquals(q, (T)0))
                {
                    T u = cbrt(-q);
                    roots[0] = 2 * u;
                    roots[1] = -u;

                    numRoots = 2;
                }
                else
                {
                    roots[0] = 0.0f;
                    numRoots = 1;
                }
            }

            T sub = THIRD * A;
            for (UINT32 i = 0; i < numRoots; i++)
                roots[i] -= sub;

            return numRoots;
        }

        /**
         * Solves the quartic equation with the parameters A, B, C, D, E. Returns number of roots found and the roots
         * themselves will be output in the @p roots array.
         *
         * @param[in]	A		First variable.
         * @param[in]	B		Second variable.
         * @param[in]	C		Third variable.
         * @param[in]	D		Fourth variable.
         * @param[in]	E		Fifth variable.
         * @param[out]	roots	Must be at least size of 4.
         *
         * @note	Only returns real roots.
         */
        template <typename T>
        static UINT32 SolveQuartic(T A, T B, T C, T D, T E, T* roots)
        {
            T invA = 1 / A;
            A = B * invA;
            B = C * invA;
            C = D * invA;
            D = E * invA;

            T sqA = A * A;
            T p = -(3 / (T)8) * sqA + B;
            T q = (1 / (T)8) * sqA * A - (T)0.5 * A * B + C;
            T r = -(3 / (T)256) * sqA * sqA + (1 / (T)16) * sqA * B - (1 / (T)4) * A * C + D;

            UINT32 numRoots = 0;
            if (!ApproxEquals(r, (T)0))
            {
                T cubicA = 1;
                T cubicB = -(T)0.5 * p;
                T cubicC = -r;
                T cubicD = (T)0.5 * r * p - (1 / (T)8) * q * q;

                SolveCubic(cubicA, cubicB, cubicC, cubicD, roots);
                T z = roots[0];

                T u = z * z - r;
                T v = 2 * z - p;

                if (ApproxEquals(u, T(0)))
                    u = 0;
                else if (u > 0)
                    u = sqrt(u);
                else
                    return 0;

                if (ApproxEquals(v, T(0)))
                    v = 0;
                else if (v > 0)
                    v = sqrt(v);
                else
                    return 0;

                T quadraticA = 1;
                T quadraticB = q < 0 ? -v : v;
                T quadraticC = z - u;

                numRoots = SolveQuadratic(quadraticA, quadraticB, quadraticC, roots);

                quadraticA = 1;
                quadraticB = q < 0 ? v : -v;
                quadraticC = z + u;

                numRoots += SolveQuadratic(quadraticA, quadraticB, quadraticC, roots + numRoots);
            }
            else
            {
                numRoots = SolveCubic(q, p, (T)0, (T)1, roots);
                roots[numRoots++] = 0;
            }

            T sub = (1 / (T)4) * A;
            for (UINT32 i = 0; i < numRoots; i++)
                roots[i] -= sub;

            return numRoots;
        }

        /**
         * Evaluates a cubic Hermite curve at a specific point.
         *
         * @param[in]	t			Parameter that at which to evaluate the curve, in range [0, 1].
         * @param[in]	pointA		Starting point (at t=0).
         * @param[in]	pointB		Ending point (at t=1).
         * @param[in]	tangentA	Starting tangent (at t=0).
         * @param[in]	tangentB	Ending tangent (at t = 1).
         * @return					Evaluated value at @p t.
         */
        template<class T>
        static T CubicHermite(float t, const T& pointA, const T& pointB, const T& tangentA, const T& tangentB)
        {
            float t2 = t * t;
            float t3 = t2 * t;

            float a = 2 * t3 - 3 * t2 + 1;
            float b = t3 - 2 * t2 + t;
            float c = -2 * t3 + 3 * t2;
            float d = t3 - t2;

            return a * pointA + b * tangentA + c * pointB + d * tangentB;
        }

        /**
         * Evaluates the first derivative of a cubic Hermite curve at a specific point.
         *
         * @param[in]	t			Parameter that at which to evaluate the curve, in range [0, 1].
         * @param[in]	pointA		Starting point (at t=0).
         * @param[in]	pointB		Ending point (at t=1).
         * @param[in]	tangentA	Starting tangent (at t=0).
         * @param[in]	tangentB	Ending tangent (at t = 1).
         * @return					Evaluated value at @p t.
         */
        template<class T>
        static T CubicHermiteD1(float t, const T& pointA, const T& pointB, const T& tangentA, const T& tangentB)
        {
            float t2 = t * t;

            float a = 6 * t2 - 6 * t;
            float b = 3 * t2 - 4 * t + 1;
            float c = -6 * t2 + 6 * t;
            float d = 3 * t2 - 2 * t;

            return a * pointA + b * tangentA + c * pointB + d * tangentB;
        }

        /**
         * Calculates coefficients needed for evaluating a cubic curve in Hermite form. Assumes @p t has been normalized is
         * in range [0, 1]. Tangents must be scaled by the length of the curve (length is the maximum value of @p t before
         * it was normalized).
         *
         * @param[in]	pointA			Starting point (at t=0).
         * @param[in]	pointB			Ending point (at t=1).
         * @param[in]	tangentA		Starting tangent (at t=0).
         * @param[in]	tangentB		Ending tangent (at t = 1).
         * @param[out]	coefficients	Four coefficients for the cubic curve, in order [t^3, t^2, t, 1].
         */
        template<class T>
        static void CubicHermiteCoefficients(const T& pointA, const T& pointB, const T& tangentA, const T& tangentB,
            T(&coefficients)[4])
        {
            T diff = pointA - pointB;

            coefficients[0] = 2 * diff + tangentA + tangentB;
            coefficients[1] = -3 * diff - 2 * tangentA - tangentB;
            coefficients[2] = tangentA;
            coefficients[3] = pointA;
        }

        /**
         * Calculates coefficients needed for evaluating a cubic curve in Hermite form. Assumes @p t is in range
         * [0, @p length]. Tangents must not be scaled by @p length.
         *
         * @param[in]	pointA			Starting point (at t=0).
         * @param[in]	pointB			Ending point (at t=length).
         * @param[in]	tangentA		Starting tangent (at t=0).
         * @param[in]	tangentB		Ending tangent (at t=length).
         * @param[in]	length			Maximum value the curve will be evaluated at.
         * @param[out]	coefficients	Four coefficients for the cubic curve, in order [t^3, t^2, t, 1].
         */
        template<class T>
        static void CubicHermiteCoefficients(const T& pointA, const T& pointB, const T& tangentA, const T& tangentB,
            float length, T(&coefficients)[4])
        {
            float length2 = length * length;
            float invLength2 = 1.0f / length2;
            float invLength3 = 1.0f / (length2 * length);

            T scaledTangentA = tangentA * length;
            T scaledTangentB = tangentB * length;

            T diff = pointA - pointB;

            coefficients[0] = (2 * diff + scaledTangentA + scaledTangentB) * invLength3;
            coefficients[1] = (-3 * diff - 2 * scaledTangentA - scaledTangentB) * invLength2;
            coefficients[2] = tangentA;
            coefficients[3] = pointA;
        }

        template <class T>
        static constexpr T Max(T a, T b) { return a > b ? a : b; }

        template <class T>
        static constexpr T Min(T a, T b) { return a < b ? a : b; }

        /** Wraps an angle so it always stays in [0, 360) range. */
        static Degree WrapAngle(Degree angle);

        // Simulate the shader function saturate that clamps a parameter value between 0 and 1
        static inline float Saturate(float t)
        {
            float tmp = std::max(t, 0.0f);
            tmp = std::min(tmp, 1.0f);
            return tmp;
        }
        static inline double Saturate(double t)
        {
            double tmp = std::max(t, 0.0);
            tmp = std::min(tmp, 1.0);
            return tmp;
        }

        static constexpr float POS_INFINITY = std::numeric_limits<float>::infinity();
        static constexpr float NEG_INFINITY = -std::numeric_limits<float>::infinity();
        static constexpr float PI = 3.14159265358979323846f;
        static constexpr float TWO_PI = (float)(2.0f * PI);
        static constexpr float HALF_PI = (float)(0.5f * PI);
        static constexpr float DEG2RAD = PI / 180.0f;
        static constexpr float RAD2DEG = 180.0f / PI;
        static const float LOG2;
    };
}
