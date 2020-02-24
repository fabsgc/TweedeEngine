#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
     /** A four dimensional vector with integer coordinates. */
    class TE_UTILITY_EXPORT Vector4I
    {
    public:
        INT32 x = 0;
        INT32 y = 0;
        INT32 z = 0;
        INT32 w = 0;

        constexpr Vector4I() = default;

        constexpr Vector4I(INT32 x, INT32 y, INT32 z, INT32 w)
            :x(x), y(y), z(z), w(w)
        { }

        INT32 operator[] (size_t i) const
        {
            assert(i < 4);

            switch (i)
            {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return 0;
            }
        }

        INT32& operator[] (size_t i)
        {
            assert(i < 4);

            switch (i)
            {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return x;
            }
        }

        bool operator== (const Vector4I& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        bool operator!= (const Vector4I& rhs) const
        {
            return !operator==(rhs);
        }
    };
}
