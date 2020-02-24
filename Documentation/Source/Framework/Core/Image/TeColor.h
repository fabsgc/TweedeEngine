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

        /**
         * Convert the current color to hue, saturation and brightness values.
         *
         * @param[in] hue			Output hue value, scaled to the [0,1] range.
         * @param[in] saturation	Output saturation level, [0,1].
         * @param[in] brightness	Output brightness level, [0,1].
         */
        void GetHSB(float* hue, float* saturation, float* brightness) const;

        /** Converts the current color from gamma to linear space and returns the result. */
        Color GetLinear() const;

        /** Converts the current color from linear to gamma space and returns the result. */
        Color GetGamma() const;

        /** Clamps color value to the range [0, 1]. */
        void Saturate()
        {
            if (r < 0)
                r = 0;
            else if (r > 1)
                r = 1;

            if (g < 0)
                g = 0;
            else if (g > 1)
                g = 1;

            if (b < 0)
                b = 0;
            else if (b > 1)
                b = 1;

            if (a < 0)
                a = 0;
            else if (a > 1)
                a = 1;
        }

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
