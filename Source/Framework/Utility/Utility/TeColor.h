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

		/** Returns the color as a 32-bit value in RGBA order. */
		RGBA GetAsRGBA() const;

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

        /** Creates a color value from a 32-bit value that encodes a RGBA color. */
		static Color FromRGBA(RGBA val);

        float r, g, b, a;
    };
}
