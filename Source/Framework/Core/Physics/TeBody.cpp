#include "TeBody.h"

namespace te
{ 
    Body::Body(const HSceneObject& linkedSO)
        : _flags(BodyFlag::None)
        , _linkedSO(linkedSO)
    { }
}
