#include "Input/TeMouse.h"
#include "Input/TeInput.h"
#include "Private/Linux/TeLinuxInput.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    struct Mouse::Pimpl
	{
		bool HasInputFocus;
	};

    Mouse::Mouse(const String& name, Input* owner)
        : _name(name)
        , _owner(owner)
    {
        _data = te_new<Pimpl>();
		_data->HasInputFocus = true;
    }

    Mouse::~Mouse()
	{
		te_delete(_data);
	}

	void Mouse::Capture()
	{
    }

    void Mouse::ChangeCaptureContext(UINT64 windowHandle)
	{
		_data->HasInputFocus = windowHandle != (UINT64)-1;
	}
}