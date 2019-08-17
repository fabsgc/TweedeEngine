//************************************ bs::framework#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
     /** A three dimensional vector with integer coordinates. */
    class TE_UTILITY_EXPORT Vector3I
    {
    public:
        INT32 x = 0;
        INT32 y = 0;
        INT32 z = 0;

        constexpr Vector3I() = default;

        constexpr Vector3I(INT32 x, INT32 y, INT32 z)
            :x(x), y(y), z(z)
        { }

        INT32 operator[] (size_t i) const
        {
            assert(i < 3);

            switch (i)
            {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return 0;
            }
        }

        INT32& operator[] (size_t i)
        {
            assert(i < 3);

            switch (i)
            {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return x;
            }
        }

        bool operator== (const Vector3I& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        bool operator!= (const Vector3I& rhs) const
        {
            return !operator==(rhs);
        }
    };
}
