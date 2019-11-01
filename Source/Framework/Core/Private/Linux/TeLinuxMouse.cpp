#include "Input/TeMouse.h"
#include "Input/TeInput.h"
#include "Private/Linux/TeLinuxInput.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    struct Mouse::Pimpl
	{
		bool HasInputFocus;
		Vector2I LastPosition;
	};

    Mouse::Mouse(const String& name, Input* owner)
        : _name(name)
        , _owner(owner)
    {
        _data = te_new<Pimpl>();
		_data->HasInputFocus = true;
		_data->LastPosition = TeZero;
    }

    Mouse::~Mouse()
	{
		te_delete(_data);
	}

	void Mouse::Capture()
	{
		if(_data->HasInputFocus)
		{
			INT16 deltaX = round(LinuxPlatform::MouseMotionEvent.DeltaX) - _data->LastPosition.x;
			INT16 deltaY = round(LinuxPlatform::MouseMotionEvent.DeltaY) - _data->LastPosition.y;
			INT16 deltaZ = 0;

			if (deltaX != 0 || deltaY != 0 || deltaZ != 0)
				_owner->NotifyMouseMoved(deltaX, deltaY, deltaZ);

			_data->LastPosition.x = LinuxPlatform::MouseMotionEvent.DeltaX;
			_data->LastPosition.y = LinuxPlatform::MouseMotionEvent.DeltaY;
		}
    }

    void Mouse::ChangeCaptureContext(UINT64 windowHandle)
	{
		_data->HasInputFocus = windowHandle != (UINT64)-1;
	}
}