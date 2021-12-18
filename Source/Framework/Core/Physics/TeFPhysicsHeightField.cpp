#include "Physics/TeFPhysicsHeightField.h"

namespace te
{
    FPhysicsHeightField::FPhysicsHeightField(const SPtr<Texture>& texture)
        : Serializable(TypeID_Core::TID_FPhysicsHeightField)
        , _texture(texture)
    { }
}