#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
    typedef UINT32 RGBA;

    /**
     * Color represented as 4 components, each being a floating point value ranging from 0 to 1. Color components are
     * red, green, blue and alpha.
     */
    class TE_UTILITY_EXPORT Color
    {
    public:
        static const Color ZERO;
        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color LightGray;

        constexpr explicit Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f )
            :r(red), g(green), b(blue), a(alpha)
        { }

        bool operator==(const Color& rhs) const;
        bool operator!=(const Color& rhs) const;

        float operator[] (const UINT32 i) const
        {
            assert(i < 4);

            return *(&r + i);
        }

        float& operator[] (const UINT32 i)
        {
            assert(i < 4);

            return *(&r + i);
        }

        /** Returns the color as a 32-bit value in RGBA order. */
        RGBA GetAsRGBA() const;

        /** Returns the color as a 4*32-bit float vector in RGBA order. */
        Vector4 GetAsVector4() const;

        /** Pointer accessor for direct copying. */
        float* Ptr()
        {
            return &r;
        }

        /** Pointer accessor for direct copying. */
        const float* Ptr() const
        {
            return &r;
        }

        Color operator+ (const Color& rhs) const
        {
            return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
        }

        Color operator- (const Color& rhs) const
        {
            return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
        }

        Color operator* (float rhs) const
        {
            return Color(rhs * r, rhs * g, rhs * b, rhs * a);
        }

        Color operator* (const Color& rhs) const
        {
            return Color(rhs.r * r, rhs.g * g, rhs.b * b, rhs.a * a);
        }

        Color operator/ (const Color& rhs) const
        {
            return Color(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);
        }

        Color operator/ (float rhs) const
        {
            assert(rhs != 0.0f);
            float invRhs = 1.0f / rhs;

            return Color(r * invRhs, g * invRhs, b * invRhs, a * invRhs);
        }

        friend Color operator* (float lhs, const Color& rhs)
        {
            return Color(lhs * rhs.r, lhs * rhs.g, lhs * rhs.b, lhs * rhs.a);
        }

        Color& operator+= (const Color& rhs)
        {
            r += rhs.r;
            g += rhs.g;
            b += rhs.b;
            a += rhs.a;

            return *this;
        }

        Color& operator-= (const Color& rhs)
        {
            r -= rhs.r;
            g -= rhs.g;
            b -= rhs.b;
            a -= rhs.a;

            return *this;
        }

        Color& operator*= (float rhs)
        {
            r *= rhs;
            g *= rhs;
            b *= rhs;
            a *= rhs;

            return *this;
        }

        Color& operator/= (float rhs)
        {
            assert(rhs != 0.0f);

            float invRhs = 1.0f / rhs;

            r *= invRhs;
            g *= invRhs;
            b *= invRhs;
            a *= invRhs;

            return *this;
        }

        /** Creates a color value from a 32-bit value that encodes a RGBA color. */
        static Color FromRGBA(RGBA val);

        float r, g, b, a;
    };
}
