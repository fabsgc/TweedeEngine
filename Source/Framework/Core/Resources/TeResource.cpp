#include "Resources/TeResource.h"

namespace te
{
    Resource::Resource(UINT32 type)
        : Serializable(type)
        , _name("Resource")
        , _path()
        , _size(0)
        , _UUID(UUID::EMPTY)
    { }
}
