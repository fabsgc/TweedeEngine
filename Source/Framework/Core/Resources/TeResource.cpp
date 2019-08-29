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
}