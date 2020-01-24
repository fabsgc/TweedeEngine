#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /**
     * Contains information about 3D object's position, rotation and scale, and provides methods to manipulate it.
     */
    class TE_CORE_EXPORT Transform
    {
    public:
        Transform() = default;
        Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
    private:
        static Transform IDENTITY;

        Vector3 _position = Vector3::ZERO;
        Quaternion _rotation = Quaternion::IDENTITY;
        Vector3 _scale = Vector3::ONE;
    };
}
