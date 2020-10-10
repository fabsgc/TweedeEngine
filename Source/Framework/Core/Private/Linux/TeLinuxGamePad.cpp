#include "Input/TeGamePad.h"
#include "Input/TeInput.h"
#include "Private/Linux/TeLinuxInput.h"
#include <fcntl.h>
#include <linux/input.h>

namespace te
{
    /** Contains private data for the Linux Gamepad implementation. */
    struct GamePad::Pimpl
    {
        GamePadInfo Info;
        INT32 FileHandle;
        ButtonCode PovState;
        bool HasInputFocus;
    };

    GamePad::GamePad(const String& name, const GamePadInfo& gamepadInfo, Input* owner)
        : _name(name)
        , _owner(owner)
    {
        _data = te_new<Pimpl>();
        _data->Info = gamepadInfo;
        _data->PovState = TE_UNASSIGNED;
        _data->HasInputFocus = true;

        String eventPath = "/dev/input/event" + ToString(gamepadInfo.EventHandlerIdx);
        _data->FileHandle = open(eventPath.c_str(), O_RDWR | O_NONBLOCK);

        if(_data->FileHandle == -1)
        {
            TE_DEBUG("Failed to open input event file handle for device");
        }
    }

    GamePad::~GamePad()
    {
        if(_data->FileHandle != -1)
        {
            close(_data->FileHandle);
        }

        te_delete(_data);
    }

    void GamePad::Capture()
    {
        if(_data->FileHandle == -1)
            return;

        struct AxisState
        {
            bool Moved;
            INT32 Value;
        };

        AxisState axisState[24];
        te_zero_out(axisState);

        input_event events[BUFFER_SIZE_GAMEPAD];
        while(true)
        {
            ssize_t numReadBytes = read(_data->FileHandle, &events, sizeof(events));
            if(numReadBytes < 0)
                break;

            if(!_data->HasInputFocus)
                continue;

            UINT32 numEvents = numReadBytes / sizeof(input_event);
            for(UINT32 i = 0; i < numEvents; ++i)
            {
                switch(events[i].type)
                {
                    case EV_KEY:
                    {
                        auto findIter = _data->Info.ButtonMap.find(events[i].code);
                        if(findIter == _data->Info.ButtonMap.end())
                            continue;

                        if(events[i].value)
                            _owner->NotifyButtonPressed(_data->Info.Id, findIter->second, (UINT64)events[i].time.tv_usec);
                        else
                            _owner->NotifyButtonReleased(_data->Info.Id, findIter->second, (UINT64)events[i].time.tv_usec);
                    }
                    break;

                    case EV_ABS:
                    {
                        // Stick or trigger
                        if(events[i].code <= ABS_BRAKE)
                        {
                            const AxisInfo& axisInfo = _data->Info.AxisMap[events[i].code];

                            if(axisInfo.AxisIdx >= 24)
                                break;

                            axisState[axisInfo.AxisIdx].Moved = true;

                            // Scale range if needed
                            if(axisInfo.Min == GamePad::MIN_AXIS && axisInfo.Max != GamePad::MAX_AXIS )
                                axisState[axisInfo.AxisIdx].Value = events[i].value;
                            else
                            {
                                float range = (float)(axisInfo.Max - axisInfo.Min);
                                float normalizedValue = (axisInfo.Max - events[i].value) / range;

                                range = (float)(GamePad::MAX_AXIS - GamePad::MIN_AXIS);
                                axisState[axisInfo.AxisIdx].Value = GamePad::MIN_AXIS + (INT32)(normalizedValue * range);
                            }
                        }
                        else if(events[i].code <= ABS_HAT3Y) // POV
                        {
                            // Note: We only support a single POV and report events from all POVs as if they were from the
                            // same source
                            INT32 povIdx = events[i].code - ABS_HAT0X;

                            ButtonCode povButton = TE_UNASSIGNED;
                            if((povIdx & 0x1) == 0) // Even, x axis
                            {
                                if(events[i].value == -1)
                                    povButton = TE_GAMEPAD_DPAD_LEFT;
                                else if(events[i].value == 1)
                                    povButton = TE_GAMEPAD_DPAD_RIGHT;
                            }
                            else // Odd, y axis
                            {
                                if(events[i].value == -1)
                                    povButton = TE_GAMEPAD_DPAD_UP;
                                else if(events[i].value == 1)
                                    povButton = TE_GAMEPAD_DPAD_DOWN;
                            }

                            if(_data->PovState != povButton)
                            {
                                if(_data->PovState != TE_UNASSIGNED)
                                    _owner->NotifyButtonReleased(_data->Info.Id, _data->PovState, (UINT64)events[i].time.tv_usec);

                                if(povButton != TE_UNASSIGNED)
                                    _owner->NotifyButtonPressed(_data->Info.Id, povButton, (UINT64)events[i].time.tv_usec);


                                _data->PovState = povButton;
                            }
                        }
                    }
                    break;

                    default:
                    break;
                }
            }
        }

        for(UINT32 i = 0; i < 24; i++)
        {
            if(axisState[i].Moved)
            {
                _owner->NotifyAxisMoved(_data->Info.Id, i, axisState[i].Value);
            }
        }
    }

    void GamePad::ChangeCaptureContext(UINT64 windowHandle)
    {
        _data->HasInputFocus = windowHandle != (UINT64)-1;
    }
}