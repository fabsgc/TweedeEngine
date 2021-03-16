#include "Input/TeVirtualInput.h"
#include "Input/TeInput.h"
#include "Math/TeMath.h"
#include "Utility/TeTime.h"

using namespace std::placeholders;

namespace te
{
    TE_MODULE_STATIC_MEMBER(VirtualInput)

    VirtualInput::VirtualInput()
        : _activeModifiers((UINT32)ButtonModifier::None)
    {
        _inputConfiguration = CreateConfiguration();

        Input::Instance().OnButtonDown.Connect(std::bind(&VirtualInput::ButtonDown, this, _1));
        Input::Instance().OnButtonUp.Connect(std::bind(&VirtualInput::ButtonUp, this, _1));
    }

    SPtr<InputConfiguration> VirtualInput::CreateConfiguration()
    {
        return te_shared_ptr_new<InputConfiguration>();
    }

    void VirtualInput::SetConfiguration(const SPtr<InputConfiguration>& input)
    {
        _inputConfiguration = input;

        // Note: Technically this is slightly wrong as it will
        // "forget" any buttons currently held down, but shouldn't matter much in practice.
        for (auto& deviceData : _devices)
            deviceData.CachedStates.clear();
    }

    bool VirtualInput::IsButtonDown(const VirtualButton& button, UINT32 deviceIdx) const
    {
        if (deviceIdx >= (UINT32)_devices.size())
            return false;

        const Map<UINT32, ButtonData>& cachedStates = _devices[deviceIdx].CachedStates;
        auto iterFind = cachedStates.find(button.ButtonIdentifier);

        if (iterFind != cachedStates.end())
            return iterFind->second.State == ButtonState::ToggledOn;

        return false;
    }

    bool VirtualInput::IsButtonUp(const VirtualButton& button, UINT32 deviceIdx) const
    {
        if (deviceIdx >= (UINT32)_devices.size())
            return false;

        const Map<UINT32, ButtonData>& cachedStates = _devices[deviceIdx].CachedStates;
        auto iterFind = cachedStates.find(button.ButtonIdentifier);

        if (iterFind != cachedStates.end())
            return iterFind->second.State == ButtonState::ToggledOff;

        return false;
    }

    bool VirtualInput::IsButtonHeld(const VirtualButton& button, UINT32 deviceIdx) const
    {
        if (deviceIdx >= (UINT32)_devices.size())
            return false;

        const Map<UINT32, ButtonData>& cachedStates = _devices[deviceIdx].CachedStates;
        auto iterFind = cachedStates.find(button.ButtonIdentifier);

        if (iterFind != cachedStates.end())
            return iterFind->second.State == ButtonState::On || iterFind->second.State == ButtonState::ToggledOn;

        return false;
    }

    float VirtualInput::GetAxisValue(const VirtualAxis& axis, UINT32 deviceIdx) const
    {
        VIRTUAL_AXIS_DESC axisDesc;
        if (_inputConfiguration->GetAxis(axis, axisDesc))
        {
            float axisValue = gInput().GetAxisValue((UINT32)axisDesc.Type, deviceIdx);

            bool isMouseAxis = (UINT32)axisDesc.Type <= (UINT32)InputAxis::MouseZ;
            bool isNormalized = axisDesc.Normalize || !isMouseAxis;

            if (isNormalized && axisDesc.DeadZone > 0.0f)
            {
                // Scale to [-1, 1] range after removing the dead zone
                if (axisValue > 0)
                    axisValue = std::max(0.f, axisValue - axisDesc.DeadZone) / (1.0f - axisDesc.DeadZone);
                else
                    axisValue = -std::max(0.f, -axisValue - axisDesc.DeadZone) / (1.0f - axisDesc.DeadZone);
            }

            if (axisDesc.Normalize)
            {
                if (isMouseAxis)
                {
                    // Currently normalizing using value of 1, which isn't doing anything, but keep the code in case that
                    // changes
                    axisValue /= 1.0f;
                }

                axisValue = Math::Clamp(axisValue * axisDesc.Sensitivity, -1.0f, 1.0f);
            }
            else
                axisValue *= axisDesc.Sensitivity;

            if (axisDesc.Invert)
                axisValue = -axisValue;

            return axisValue;
        }

        return 0.0f;
    }

    void VirtualInput::Update()
    {
        UINT64 frameIdx = gTime().GetFrameIdx();
        for (auto& deviceData : _devices)
        {
            for (auto& state : deviceData.CachedStates)
            {
                // We need to stay in toggled state for one frame.
                if (state.second.UpdateFrameIdx == frameIdx)
                    continue;

                if (state.second.State == ButtonState::ToggledOff)
                    state.second.State = ButtonState::Off;
                else if (state.second.State == ButtonState::ToggledOn)
                    state.second.State = ButtonState::On;
            }
        }

        bool hasEvents = true;
        UINT64 repeatInternal = _inputConfiguration->GetRepeatInterval();
        UINT64 currentTime = gTime().GetTimeMs();

        // Trigger all events
        while (hasEvents)
        {
            while (!_events.empty())
            {
                VirtualButtonEvent& event = _events.front();

                if (event.State == ButtonState::On)
                {
                    if (!OnButtonDown.Empty())
                        OnButtonDown(event.Button, event.DeviceIdx);
                }
                else if (event.State == ButtonState::Off)
                {
                    if (!OnButtonUp.Empty())
                        OnButtonUp(event.Button, event.DeviceIdx);
                }

                _events.pop();
            }

            // Queue up any repeatable events
            hasEvents = false;

            for (auto& deviceData : _devices)
            {
                for (auto& state : deviceData.CachedStates)
                {
                    if (state.second.State != ButtonState::On)
                        continue;

                    if (state.second.State == ButtonState::On)
                    {
                        UINT32 identifier = state.second.Button.ButtonIdentifier;
                        Vector<VirtualButton> btns;
                        Vector<VIRTUAL_BUTTON_DESC> btnDesc;

                        if (!gInput().IsButtonHeld(ButtonCode::TE_LCONTROL))
                        {
                            _inputConfiguration->GetButtons(ButtonCode::TE_LCONTROL, 0, btns, btnDesc);
                            for (auto& btn : btns)
                            {
                                if(state.first == btn.ButtonIdentifier)
                                    deviceData.CachedStates[btn.ButtonIdentifier].State = ButtonState::Off;
                            } 
                        }

                        if (!gInput().IsButtonHeld(ButtonCode::TE_LMENU))
                        {
                            _inputConfiguration->GetButtons(ButtonCode::TE_LMENU, 0, btns, btnDesc);
                            for (auto& btn : btns)
                                deviceData.CachedStates[btn.ButtonIdentifier].State = ButtonState::Off;
                        }

                        if (!gInput().IsButtonHeld(ButtonCode::TE_DELETE))
                        {
                            _inputConfiguration->GetButtons(ButtonCode::TE_DELETE, 0, btns, btnDesc);
                            for (auto& btn : btns)
                                deviceData.CachedStates[btn.ButtonIdentifier].State = ButtonState::Off;
                        }
                    }

                    if (!state.second.AllowRepeat)
                        continue;

                    UINT64 diff = currentTime - state.second.Timestamp;
                    if (diff >= repeatInternal)
                    {
                        state.second.Timestamp += repeatInternal;

                        VirtualButtonEvent event;
                        event.Button = state.second.Button;
                        event.State = ButtonState::On;
                        event.DeviceIdx = 0;

                        _events.push(event);
                        hasEvents = true;
                    }
                }

                break; // Only repeat the first device. Repeat only makes sense for keyboard which there is only one of.
            }
        }
    }

    void VirtualInput::ButtonDown(const ButtonEvent& event)
    {
        if (event.buttonCode == TE_LSHIFT || event.buttonCode == TE_RSHIFT)
            _activeModifiers |= (UINT32)ButtonModifier::Shift;
        else if (event.buttonCode == TE_LCONTROL || event.buttonCode == TE_RCONTROL)
            _activeModifiers |= (UINT32)ButtonModifier::Ctrl;
        else if (event.buttonCode == TE_LMENU || event.buttonCode == TE_RMENU)
            _activeModifiers |= (UINT32)ButtonModifier::Alt;

        _tempButtons.clear();
        _tempBtnDescs.clear();

        if (_inputConfiguration->GetButtons(event.buttonCode, _activeModifiers, _tempButtons, _tempBtnDescs))
        {
            while (event.deviceIdx >= (UINT32)_devices.size())
                _devices.push_back(DeviceData());

            Map<UINT32, ButtonData>& cachedStates = _devices[event.deviceIdx].CachedStates;

            UINT32 numButtons = (UINT32)_tempButtons.size();
            for (UINT32 i = 0; i < numButtons; i++)
            {
                const VirtualButton& btn = _tempButtons[i];
                const VIRTUAL_BUTTON_DESC& btnDesc = _tempBtnDescs[i];

                ButtonData& data = cachedStates[btn.ButtonIdentifier];

                data.Button = btn;
                data.State = ButtonState::ToggledOn;
                data.Timestamp = event.timestamp;
                data.UpdateFrameIdx = gTime().GetFrameIdx();
                data.AllowRepeat = btnDesc.Repeatable;

                VirtualButtonEvent virtualEvent;
                virtualEvent.Button = btn;
                virtualEvent.State = ButtonState::On;
                virtualEvent.DeviceIdx = event.deviceIdx;

                _events.push(virtualEvent);
            }
        }
    }

    void VirtualInput::ButtonUp(const ButtonEvent& event)
    {
        if (event.buttonCode == TE_LSHIFT || event.buttonCode == TE_RSHIFT)
            _activeModifiers &= ~(UINT32)ButtonModifier::Shift;
        else if (event.buttonCode == TE_LCONTROL || event.buttonCode == TE_RCONTROL)
            _activeModifiers &= ~(UINT32)ButtonModifier::Ctrl;
        else if (event.buttonCode == TE_LMENU || event.buttonCode == TE_RMENU)
            _activeModifiers &= ~(UINT32)ButtonModifier::Alt;

        _tempButtons.clear();
        _tempBtnDescs.clear();

        if (_inputConfiguration->GetButtons(event.buttonCode, _activeModifiers, _tempButtons, _tempBtnDescs))
        {
            while (event.deviceIdx >= (UINT32)_devices.size())
                _devices.push_back(DeviceData());

            Map<UINT32, ButtonData>& cachedStates = _devices[event.deviceIdx].CachedStates;

            UINT32 numButtons = (UINT32)_tempButtons.size();
            for (UINT32 i = 0; i < numButtons; i++)
            {
                const VirtualButton& btn = _tempButtons[i];
                const VIRTUAL_BUTTON_DESC& btnDesc = _tempBtnDescs[i];

                ButtonData& data = cachedStates[btn.ButtonIdentifier];

                data.Button = btn;
                data.State = ButtonState::ToggledOff;
                data.Timestamp = event.timestamp;
                data.UpdateFrameIdx = gTime().GetFrameIdx();
                data.AllowRepeat = btnDesc.Repeatable;

                VirtualButtonEvent virtualEvent;
                virtualEvent.Button = btn;
                virtualEvent.State = ButtonState::Off;
                virtualEvent.DeviceIdx = event.deviceIdx;

                _events.push(virtualEvent);
            }
        }
    }

    VirtualInput& gVirtualInput()
    {
        return VirtualInput::Instance();
    }
}
