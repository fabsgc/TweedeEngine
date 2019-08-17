#include "Input/TeKeyboard.h"
#include "Input/TeInput.h"
#include "Private/Linux/TeLinuxInput.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    struct Keyboard::Pimpl
	{
		bool HasInputFocus;
	};

    Keyboard::Keyboard(const String& name, Input* owner)
        : _name(name)
        , _owner(owner)
    {
        _data = te_new<Pimpl>();
		_data->HasInputFocus = true;
    }

    Keyboard::~Keyboard()
    {
        te_delete(_data);
    }

    void Keyboard::Capture()
	{
    }

    void Keyboard::ChangeCaptureContext(UINT64 windowHandle)
	{
		_data->HasInputFocus = windowHandle != (UINT64)-1;
	}
}