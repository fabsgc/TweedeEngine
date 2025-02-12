#include "Physics/TeFPhysicsHeightField.h"

namespace te
{
    FPhysicsHeightField::FPhysicsHeightField(const SPtr<Texture>& texture)
        : Serializable(CoreType::TID_FPhysicsHeightField)
        , _texture(texture)
    { }
}