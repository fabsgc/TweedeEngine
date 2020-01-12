#include "Resources/TeResource.h"

namespace te
{
    const String& Resource::GetName() const
	{
		return _name;
	}

	void Resource::SetName(const String& name)
	{
		_name = name;
	}
}