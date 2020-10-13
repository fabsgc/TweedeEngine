#include "Input/TeInput.h"
#include "Input/TeMouse.h"
#include "Input/TeKeyboard.h"
#include "Input/TeGamePad.h"
#include "Platform/TePlatform.h"
#include "TeCoreApplication.h"
#include "Utility/TeTime.h"

using namespace std::placeholders;

namespace te
{   
    TE_MODULE_STATIC_MEMBER(Input)

    Input::DeviceData::DeviceData()
    {
        for (UINT32 i = 0; i < TE_Count; i++)
            KeyStates[i] = ButtonState::Off;
    }

    Input::Input()
        : _mouse(nullptr)
        , _keyboard(nullptr)
        , _platformData(nullptr)
    {
        SPtr<RenderWindow> window = gCoreApplication().GetWindow();
        window->GetCustomAttribute("WINDOW", &_windowHandle);

        _charInputConn = Platform::OnCharInput.Connect(std::bind(&Input::CharInput, this, _1));
        _cursorMovedConn = Platform::OnCursorMoved.Connect(std::bind(&Input::CursorMoved, this, _1, _2));
        _cursorPressedConn = Platform::OnCursorButtonPressed.Connect(std::bind(&Input::CursorPressed, this, _1, _2, _3));
        _cursorReleasedConn = Platform::OnCursorButtonReleased.Connect(std::bind(&Input::CursorReleased, this, _1, _2, _3));
        _cursorDoubleClickConn = Platform::OnCursorDoubleClick.Connect(std::bind(&Input::CursorDoubleClick, this, _1, _2));
        _inputCommandConn = Platform::OnInputCommand.Connect(std::bind(&Input::InputCommandEntered, this, _1));

        _mouseWheelScrolledConn = Platform::OnMouseWheelScrolled.Connect(std::bind(&Input::MouseWheelScrolled, this, _1));

        auto focusGainListener = std::bind(&Input::InputWindowChanged, this, _1);
        auto focusLostListener = std::bind(&Input::InputFocusLost, this);

        gCoreApplication().GetWindow()->OnFocusGained.Connect(std::move(focusGainListener));
        gCoreApplication().GetWindow()->OnFocusLost.Connect(std::move(focusLostListener));

        for (int i = 0; i < 3; i++)
            _pointerButtonStates[i] = ButtonState::Off;

        _mouseSampleAccumulator[0] = 0;
        _mouseSampleAccumulator[1] = 0;
        _totalMouseSamplingTime[0] = 1.0f / 125.0f; // Use 125Hz as initial pooling rate for mice
        _totalMouseSamplingTime[1] = 1.0f / 125.0f;
        _totalMouseNumSamples[0] = 1;
        _totalMouseNumSamples[1] = 1;
        _mouseSmoothedAxis[0] = 0.0f;
        _mouseSmoothedAxis[1] = 0.0f;
        _mouseZeroTime[0] = 0.0f;
        _mouseZeroTime[1] = 0.0f;

        InitRawInput();
    }

    Input::~Input()
    {
        CleanUpRawInput();

        _charInputConn.Disconnect();
        _cursorMovedConn.Disconnect();
        _inputCommandConn.Disconnect();
        _cursorPressedConn.Disconnect();
        _cursorReleasedConn.Disconnect();
        _cursorDoubleClickConn.Disconnect();
        _mouseWheelScrolledConn.Disconnect();
    }

    void Input::Update()
    {
        // Toggle states only remain active for a single frame before they are transitioned
        // into permanent state

        for (auto& deviceData : _devices)
        {
            for (UINT32 i = 0; i < TE_Count; i++)
            {
                if (deviceData.KeyStates[i] == ButtonState::ToggledOff || deviceData.KeyStates[i] == ButtonState::ToggledOnOff)
                    deviceData.KeyStates[i] = ButtonState::Off;
                else if (deviceData.KeyStates[i] == ButtonState::ToggledOn)
                    deviceData.KeyStates[i] = ButtonState::On;
            }

            UINT32 numAxes = (UINT32)deviceData.Axes.size();
            for (UINT32 i = 0; i < numAxes; i++)
                deviceData.Axes[i] = 0.0f;
        }

        for (UINT32 i = 0; i < 3; i++)
        {
            if (_pointerButtonStates[i] == ButtonState::ToggledOff || _pointerButtonStates[i] == ButtonState::ToggledOnOff)
                _pointerButtonStates[i] = ButtonState::Off;
            else if (_pointerButtonStates[i] == ButtonState::ToggledOn)
                _pointerButtonStates[i] = ButtonState::On;
        }

        _pointerDelta = Vector2I::ZERO; // Reset delta in case we don't receive any mouse input this frame
        _pointerDoubleClicked = false;

        // Capture raw input
        if (_mouse != nullptr)
            _mouse->Capture();

        if (_keyboard != nullptr)
            _keyboard->Capture();

        for (auto& gamepad : _gamepads)
            gamepad->Capture();

        float rawXValue = (float)_mouseSampleAccumulator[0];
        float rawYValue = (float)_mouseSampleAccumulator[1];

        rawXValue *= 0.1f;
        rawYValue *= 0.1f;

        _mouseSampleAccumulator[0] = 0;
        _mouseSampleAccumulator[1] = 0;

        AxisMoved(0, -rawXValue, (UINT32)InputAxis::MouseX);
        AxisMoved(0, -rawYValue, (UINT32)InputAxis::MouseY);
    }

    void Input::TriggerCallbacks()
    {
        Vector2I pointerPos;
        float mouseScroll;
        OSPointerButtonStates pointerState;

        {
            std::swap(_queuedEvents[0], _queuedEvents[1]);

            std::swap(_buttonDownEvents[0], _buttonDownEvents[1]);
            std::swap(_buttonUpEvents[0], _buttonUpEvents[1]);

            std::swap(_pointerPressedEvents[0], _pointerPressedEvents[1]);
            std::swap(_pointerReleasedEvents[0], _pointerReleasedEvents[1]);
            std::swap(_pointerDoubleClickEvents[0], _pointerDoubleClickEvents[1]);

            std::swap(_textInputEvents[0], _textInputEvents[1]);

            pointerPos = _pointerPosition;
            mouseScroll = _mouseScroll;
            pointerState = _pointerState;

            _mouseScroll = 0.0f;
        }

        if (pointerPos != _lastPointerPosition || mouseScroll != 0.0f)
        {
            PointerEvent event;
            event.alt = false;
            event.shift = pointerState.Shift;
            event.control = pointerState.Ctrl;
            event.buttonStates[0] = pointerState.MouseButtons[0];
            event.buttonStates[1] = pointerState.MouseButtons[1];
            event.buttonStates[2] = pointerState.MouseButtons[2];
            event.mouseWheelScrollAmount = mouseScroll;

            event.type = PointerEventType::CursorMoved;
            event.screenPos = pointerPos;

            if (_lastPositionSet)
                _pointerDelta = event.screenPos - _lastPointerPosition;

            event.delta = _pointerDelta;

            OnPointerMoved(event);

            _lastPointerPosition = event.screenPos;
            _lastPositionSet = true;
        }

        for (auto& event : _queuedEvents[1])
        {
            switch (event.Type)
            {
            case EventType::ButtonDown:
            {
                const ButtonEvent& eventData = _buttonDownEvents[1][event.Idx];

                _devices[eventData.deviceIdx].KeyStates[eventData.buttonCode & 0x0000FFFF] = ButtonState::ToggledOn;
                OnButtonDown(_buttonDownEvents[1][event.Idx]);
            }
            break;
            case EventType::ButtonUp:
            {
                const ButtonEvent& eventData = _buttonUpEvents[1][event.Idx];

                while (eventData.deviceIdx >= (UINT32)_devices.size())
                    _devices.push_back(DeviceData());

                if (_devices[eventData.deviceIdx].KeyStates[eventData.buttonCode & 0x0000FFFF] == ButtonState::ToggledOn)
                    _devices[eventData.deviceIdx].KeyStates[eventData.buttonCode & 0x0000FFFF] = ButtonState::ToggledOnOff;
                else
                    _devices[eventData.deviceIdx].KeyStates[eventData.buttonCode & 0x0000FFFF] = ButtonState::ToggledOff;

                OnButtonUp(_buttonUpEvents[1][event.Idx]);
            }
            break;
            case EventType::PointerDown:
            {
                const PointerEvent& eventData = _pointerPressedEvents[1][event.Idx];
                _pointerButtonStates[(UINT32)eventData.button] = ButtonState::ToggledOn;

                OnPointerPressed(eventData);
            }
            break;
            case EventType::PointerUp:
            {
                const PointerEvent& eventData = _pointerReleasedEvents[1][event.Idx];

                if (_pointerButtonStates[(UINT32)eventData.button] == ButtonState::ToggledOn)
                    _pointerButtonStates[(UINT32)eventData.button] = ButtonState::ToggledOnOff;
                else
                    _pointerButtonStates[(UINT32)eventData.button] = ButtonState::ToggledOff;

                OnPointerReleased(eventData);
            }
            break;
            case EventType::PointerDoubleClick:
                _pointerDoubleClicked = true;
                OnPointerDoubleClick(_pointerDoubleClickEvents[1][event.Idx]);
                break;
            case EventType::TextInput:
                OnCharInput(_textInputEvents[1][event.Idx]);
                break;
            case EventType::Command:
                OnInputCommand(_commandEvents[1][event.Idx]);
                break;
            default:
                break;
            }
        }

        _queuedEvents[1].clear();
        _buttonDownEvents[1].clear();
        _buttonUpEvents[1].clear();
        _pointerPressedEvents[1].clear();
        _pointerReleasedEvents[1].clear();
        _pointerDoubleClickEvents[1].clear();
        _textInputEvents[1].clear();
    }

    void Input::InputWindowChanged(RenderWindow& win)
    {
        UINT64 hWnd = 0;
        win.GetCustomAttribute("WINDOW", &hWnd);

        if(_keyboard != nullptr)
            _keyboard->ChangeCaptureContext(hWnd);
        
        if(_mouse != nullptr)
            _mouse->ChangeCaptureContext(hWnd);

        for (auto& gamepad : _gamepads)
        {
            gamepad->ChangeCaptureContext(hWnd);
        } 
    }

    void Input::InputFocusLost()
    {
        if(_keyboard != nullptr)
            _keyboard->ChangeCaptureContext((UINT64)-1);

        if(_mouse != nullptr)
            _mouse->ChangeCaptureContext((UINT64)-1);

        for (auto& gamepad : _gamepads)
        {
            gamepad->ChangeCaptureContext((UINT64)-1);
        }
    }

    Vector2I Input::GetPointerPosition() const
    {
        return _pointerPosition;
    }

    float Input::GetAxisValue(UINT32 type, UINT32 deviceIdx) const
    {
        if (deviceIdx >= (UINT32)_devices.size())
            return 0.0f;

        const Vector<float>& axes = _devices[deviceIdx].Axes;
        if (type >= (UINT32)axes.size())
            return 0.0f;

        return axes[type];
    }

    bool Input::IsButtonHeld(ButtonCode button, UINT32 deviceIdx) const
    {
        if (deviceIdx >= (UINT32)_devices.size())
            return false;

        return _devices[deviceIdx].KeyStates[button & 0x0000FFFF] == ButtonState::On ||
            _devices[deviceIdx].KeyStates[button & 0x0000FFFF] == ButtonState::ToggledOn ||
            _devices[deviceIdx].KeyStates[button & 0x0000FFFF] == ButtonState::ToggledOnOff;
    }

    bool Input::IsButtonUp(ButtonCode button, UINT32 deviceIdx) const
    {
        if (deviceIdx >= (UINT32)_devices.size())
            return false;

        return _devices[deviceIdx].KeyStates[button & 0x0000FFFF] == ButtonState::ToggledOff ||
            _devices[deviceIdx].KeyStates[button & 0x0000FFFF] == ButtonState::ToggledOnOff;
    }

    bool Input::IsButtonDown(ButtonCode button, UINT32 deviceIdx) const
    {
        if (deviceIdx >= (UINT32)_devices.size())
            return false;

        return _devices[deviceIdx].KeyStates[button & 0x0000FFFF] == ButtonState::ToggledOn ||
            _devices[deviceIdx].KeyStates[button & 0x0000FFFF] == ButtonState::ToggledOnOff;
    }

    bool Input::IsPointerButtonHeld(PointerEventButton pointerButton) const
    {
        return _pointerButtonStates[(UINT32)pointerButton] == ButtonState::On ||
            _pointerButtonStates[(UINT32)pointerButton] == ButtonState::ToggledOn ||
            _pointerButtonStates[(UINT32)pointerButton] == ButtonState::ToggledOnOff;
    }

    bool Input::IsPointerButtonUp(PointerEventButton pointerButton) const
    {
        return _pointerButtonStates[(UINT32)pointerButton] == ButtonState::ToggledOff ||
            _pointerButtonStates[(UINT32)pointerButton] == ButtonState::ToggledOnOff;
    }

    bool Input::IsPointerButtonDown(PointerEventButton pointerButton) const
    {
        return _pointerButtonStates[(UINT32)pointerButton] == ButtonState::ToggledOn ||
            _pointerButtonStates[(UINT32)pointerButton] == ButtonState::ToggledOnOff;
    }

    bool Input::IsPointerDoubleClicked() const
    {
        return _pointerDoubleClicked;
    }

    void Input::NotifyMouseMoved(INT32 relX, INT32 relY, INT32 relZ)
    {
        _mouseSampleAccumulator[0] += relX;
        _mouseSampleAccumulator[1] += relY;

        _totalMouseNumSamples[0] += Math::RoundToInt(Math::Abs((float)relX));
        _totalMouseNumSamples[1] += Math::RoundToInt(Math::Abs((float)relY));

        // Update sample times used for determining sampling rate. But only if something was
        // actually sampled, and only if this isn't the first non-zero sample.
        if (_lastMouseUpdateFrame != gTime().GetFrameIdx())
        {
            if (relX != 0 && !Math::ApproxEquals(_mouseSmoothedAxis[0], 0.0f))
                _totalMouseSamplingTime[0] += gTime().GetFrameDelta();

            if (relY != 0 && !Math::ApproxEquals(_mouseSmoothedAxis[1], 0.0f))
                _totalMouseSamplingTime[1] += gTime().GetFrameDelta();

            _lastMouseUpdateFrame = gTime().GetFrameIdx();
        }

        AxisMoved(0, (float)relZ, (UINT32)InputAxis::MouseZ);

        OnPointerRelativeMoved(Vector2I(relX, relY));
    }

    void Input::NotifyAxisMoved(UINT32 gamepadIdx, UINT32 axisIdx, INT32 value)
    {
        // Move axis values into [-1.0f, 1.0f] range
        float axisRange = Math::Abs((float)GamePad::MAX_AXIS) + Math::Abs((float)GamePad::MIN_AXIS);

        float axisValue = ((value + Math::Abs((float)GamePad::MIN_AXIS)) / axisRange) * 2.0f - 1.0f;
        AxisMoved(gamepadIdx, axisValue, axisIdx);
    }

    void Input::NotifyButtonPressed(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp)
    {
        // TE_PRINT("Button pressed : " + ToString(code) + ":" + ToString(timestamp));
        ButtonDown(deviceIdx, code, timestamp);
    }

    void Input::NotifyButtonReleased(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp)
    {
        // TE_PRINT("Button released : " + ToString(code) + ":" + ToString(timestamp));
        ButtonUp(deviceIdx, code, timestamp);
    }

    void Input::CharInput(UINT32 chr)
    {
        // TE_PRINT("Char input : " + ToString(chr));

        TextInputEvent textInputEvent;
        textInputEvent.textChar = chr;

        _queuedEvents[0].push_back(QueuedEvent(EventType::TextInput, (UINT32)_textInputEvents[0].size()));
        _textInputEvents[0].push_back(textInputEvent);
    }

    void Input::CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    {
        _pointerPosition = cursorPos;
        _pointerState = btnStates;
    }

    void Input::CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    {
        PointerEvent event;
        event.alt = false;
        event.shift = btnStates.Shift;
        event.control = btnStates.Ctrl;
        event.buttonStates[0] = btnStates.MouseButtons[0];
        event.buttonStates[1] = btnStates.MouseButtons[1];
        event.buttonStates[2] = btnStates.MouseButtons[2];

        switch (button)
        {
        case OSMouseButton::Left:
            event.button = PointerEventButton::Left;
            break;
        case OSMouseButton::Middle:
            event.button = PointerEventButton::Middle;
            break;
        case OSMouseButton::Right:
            event.button = PointerEventButton::Right;
            break;
        default:
            break;
        }

        event.screenPos = cursorPos;
        event.type = PointerEventType::ButtonPressed;

        _queuedEvents[0].push_back(QueuedEvent(EventType::PointerDown, (UINT32)_pointerPressedEvents[0].size()));
        _pointerPressedEvents[0].push_back(event);
    }

    void Input::CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    {
        PointerEvent event;
        event.alt = false;
        event.shift = btnStates.Shift;
        event.control = btnStates.Ctrl;
        event.buttonStates[0] = btnStates.MouseButtons[0];
        event.buttonStates[1] = btnStates.MouseButtons[1];
        event.buttonStates[2] = btnStates.MouseButtons[2];

        switch (button)
        {
        case OSMouseButton::Left:
            event.button = PointerEventButton::Left;
            break;
        case OSMouseButton::Middle:
            event.button = PointerEventButton::Middle;
            break;
        case OSMouseButton::Right:
            event.button = PointerEventButton::Right;
            break;
        default:
            break;
        }

        event.screenPos = cursorPos;
        event.type = PointerEventType::ButtonReleased;

        _queuedEvents[0].push_back(QueuedEvent(EventType::PointerUp, (UINT32)_pointerReleasedEvents[0].size()));
        _pointerReleasedEvents[0].push_back(event);
    }

    void Input::CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    {
        PointerEvent event;
        event.alt = false;
        event.shift = btnStates.Shift;
        event.control = btnStates.Ctrl;
        event.buttonStates[0] = btnStates.MouseButtons[0];
        event.buttonStates[1] = btnStates.MouseButtons[1];
        event.buttonStates[2] = btnStates.MouseButtons[2];
        event.button = PointerEventButton::Left;
        event.screenPos = cursorPos;
        event.type = PointerEventType::DoubleClick;

        _queuedEvents[0].push_back(QueuedEvent(EventType::PointerDoubleClick, (UINT32)_pointerDoubleClickEvents[0].size()));
        _pointerDoubleClickEvents[0].push_back(event);
    }

    void Input::InputCommandEntered(InputCommandType commandType)
    {
        _queuedEvents[0].push_back(QueuedEvent(EventType::Command, (UINT32)_commandEvents[0].size()));
        _commandEvents[0].push_back(commandType);
    }

    void Input::MouseWheelScrolled(float scrollPos)
    {
        _mouseScroll = scrollPos;
    }

    void Input::ButtonDown(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp)
    {
        while (deviceIdx >= (UINT32)_devices.size())
        {
            _devices.push_back(DeviceData());
        }

        ButtonEvent btnEvent;
        btnEvent.buttonCode = code;
        btnEvent.timestamp = timestamp;
        btnEvent.deviceIdx = deviceIdx;

        _queuedEvents[0].push_back(QueuedEvent(EventType::ButtonDown, (UINT32)_buttonDownEvents[0].size()));
        _buttonDownEvents[0].push_back(btnEvent);
    }

    void Input::ButtonUp(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp)
    {
        ButtonEvent btnEvent;
        btnEvent.buttonCode = code;
        btnEvent.timestamp = timestamp;
        btnEvent.deviceIdx = deviceIdx;

        _queuedEvents[0].push_back(QueuedEvent(EventType::ButtonUp, (UINT32)_buttonUpEvents[0].size()));
        _buttonUpEvents[0].push_back(btnEvent);
    }

    void Input::AxisMoved(UINT32 deviceIdx, float value, UINT32 axis)
    {
        // Note: This method must only ever be called from the main thread, as we don't lock access to axis data
        while (deviceIdx >= (UINT32)_devices.size())
            _devices.push_back(DeviceData());

        Vector<float>& axes = _devices[deviceIdx].Axes;
        while (axis >= (UINT32)axes.size())
            axes.push_back(0.0f);

        _devices[deviceIdx].Axes[axis] = value;
    }

    Input& gInput()
    {
        return Input::Instance();
    }

    Input* gInputPtr()
    {
        return Input::InstancePtr();
    }
}