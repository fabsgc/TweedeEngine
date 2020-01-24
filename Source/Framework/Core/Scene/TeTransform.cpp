#include "TeTransform.h"

namespace te
{
        Transform Transform::IDENTITY;

        Transform::Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
        : _position(position), _rotation(rotation), _scale(scale)
    { }
}
