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
		if(_data->HasInputFocus)
		{
			double deltaX = round(LinuxPlatform::MouseMotionEvent.DeltaX);
			double deltaY = round(LinuxPlatform::MouseMotionEvent.DeltaY);
			double deltaZ = round(LinuxPlatform::MouseMotionEvent.DeltaZ);

			if (deltaX != 0 || deltaY != 0 || deltaZ != 0)
				_owner->NotifyMouseMoved(deltaX, deltaY, deltaZ);

			LinuxPlatform::MouseMotionEvent.DeltaX -= deltaX;
			LinuxPlatform::MouseMotionEvent.DeltaY -= deltaY;
			LinuxPlatform::MouseMotionEvent.DeltaZ -= deltaZ;
		}
		else
		{
			// Discard accumulated data
			LinuxPlatform::MouseMotionEvent.DeltaX = 0;
			LinuxPlatform::MouseMotionEvent.DeltaY = 0;
			LinuxPlatform::MouseMotionEvent.DeltaZ = 0;
		}
    }

    void Mouse::ChangeCaptureContext(UINT64 windowHandle)
	{
		_data->HasInputFocus = windowHandle != (UINT64)-1;
	}
}