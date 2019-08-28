#include "Resources/TeResource.h"

namespace te
{
    const String& Resource::getName() const
	{
		return _name;
	}

	void Resource::setName(const String& name)
	{
		_name = name;
	}

    const te::UUID& Resource::getUUID() const
    {
        return _uuid;
    }

    void Resource::setUUID(const te::UUID& uuid)
    {
        _uuid = uuid;
    }
}