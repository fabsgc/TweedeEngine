#include "Math/TeVector2.h"

namespace te
{
    const Vector2 Vector2::ZERO(TeZero);
    const Vector2 Vector2::ONE(1, 1);
    const Vector2 Vector2::UNIT_X(1, 0);
    const Vector2 Vector2::UNIT_Y(0, 1);

    void Vector2::ExportJson(nlohmann::json& document) const
    {
        document = { { "x", x }, { "y", y } };
    }

    Vector2 Vector2::ImportJson(const nlohmann::json& document)
    {
        return Vector2(
            document["x"].get<float>(),
            document["y"].get<float>()
        );
    }
}
