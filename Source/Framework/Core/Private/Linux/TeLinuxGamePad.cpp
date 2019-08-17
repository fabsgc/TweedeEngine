#include "Input/TeGamePad.h"
#include "Input/TeInput.h"
#include "Private/Linux/TeLinuxInput.h"

namespace te
{
    /** Contains private data for the Linux Gamepad implementation. */
	struct GamePad::Pimpl
	{
        bool HasInputFocus;
    };

    GamePad::GamePad(const String& name, const GamePadInfo& gamepadInfo, Input* owner)
        : _name(name)
        , _owner(owner)
    {
        _data = te_new<Pimpl>();
    }

    GamePad::~GamePad()
    {
        te_delete(_data);
    }

    void GamePad::Capture()
	{
    }

    void GamePad::ChangeCaptureContext(UINT64 windowHandle)
	{
		_data->HasInputFocus = windowHandle != (UINT64)-1;
	}
}