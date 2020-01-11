#include "TeCoreObject.h"

namespace te
{
    void CoreObject::SetThisPtr(SPtr<CoreObject> ptrThis)
	{
		_this = ptrThis;
	}
}
