#include "Math/TeVector4.h"
#include "Math/TeMath.h"

namespace te
{
    const Vector4 Vector4::ZERO{ TE_ZERO() };

    bool Vector4::IsNaN() const
    {
        return Math::IsNaN(x) || Math::IsNaN(y) || Math::IsNaN(z) || Math::IsNaN(w);
    }
}
