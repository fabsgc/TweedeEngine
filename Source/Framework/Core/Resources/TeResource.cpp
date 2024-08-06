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

    void Resource::Serialize(StreamWriter* serializer) const
    {
        Serializable::Serialize(serializer);

        serializer->WriteString(_name);
        serializer->WriteString(_path);
        serializer->WriteRaw<UINT32>(_size);
        serializer->WriteString(_UUID.ToString());
    }
}
