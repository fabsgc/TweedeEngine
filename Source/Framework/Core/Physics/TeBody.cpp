#include "TeBody.h"
#include "Scene/TeSceneObject.h"

namespace te
{ 
    Body::Body(const HSceneObject& linkedSO)
        : _flags(BodyFlag::None)
        , _linkedSO(linkedSO)
    { }

    void Body::_setTransform(const Vector3& position, const Quaternion& rotation)
    {
        _linkedSO->SetWorldPosition(position);
        _linkedSO->SetWorldRotation(rotation);
    }
}
