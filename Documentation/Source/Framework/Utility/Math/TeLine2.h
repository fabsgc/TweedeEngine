#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeVector2.h"

namespace te
{
     /** A line in 2D space represented with an origin and direction. */
    class TE_UTILITY_EXPORT Line2
    {
    public:
        Line2() = default;

        Line2(const Vector2& origin, const Vector2& direction)
            : _origin(origin), _direction(direction)
        { }

        void SetOrigin(const Vector2& origin) { _origin = origin; }
        const Vector2& GetOrigin(void) const { return _origin; }

        void SetDirection(const Vector2& dir) { _direction = dir; }
        const Vector2& GetDirection(void) const { return _direction; }

        /** Gets the position of a point t units along the line. */
        Vector2 GetPoint(float t) const
        {
            return Vector2(_origin + (_direction * t));
        }

        /** Gets the position of a point t units along the line. */
        Vector2 operator*(float t) const
        {
            return GetPoint(t);
        }

        /** Line/Line intersection, returns boolean result and distance to intersection point. */
        std::pair<bool, float> Intersects(const Line2& line) const;

    protected:
        Vector2 _origin = Vector2::ZERO;
        Vector2 _direction = Vector2::UNIT_X;
    };
}
