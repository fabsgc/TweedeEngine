#include "Resources/TeResource.h"

namespace te
{
    Resource::Resource(UINT32 type)
        : Serializable(type)
        , _name("Resource")
    { }

    const String& Resource::GetName() const
    {
        return _name;
    }

    void Resource::SetName(const String& name)
    {
        _name = name;
    }
}
