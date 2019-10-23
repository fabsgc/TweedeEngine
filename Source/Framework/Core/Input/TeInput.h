#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TeEvent.h"
#include "Platform/TePlatform.h"
#include "Math/TeVector2I.h"
#include "Input/TeInputData.h"

namespace te
{
    class Mouse;
    class Keyboard;
    class Gamepad;
    struct InputPrivateData;

    class TE_CORE_EXPORT Input : public Module<Input>
    {
        /** Possible button states. */
        enum class ButtonState
        {
            Off, /**< Button is not being pressed. */
            On, /**< Button is being pressed. */
            ToggledOn, /**< Button has been pressed this frame. */
            ToggledOff, /**< Button has been released this frame. */
            ToggledOnOff, /**< Button has been pressed and released this frame. */
        };

        /** Contains axis and device data per device. */
        struct DeviceData
        {
            DeviceData();

            Vector<float> Axes;
            ButtonState KeyStates[TE_Count];
        };

        /**	Different types of possible input event callbacks. */
        enum class EventType
        {
            ButtonUp, ButtonDown, PointerMoved, PointerUp, PointerDown, PointerDoubleClick, TextInput
        };

        /**	Stores information about a queued input event that is to be triggered later. */
        struct QueuedEvent
        {
            QueuedEvent(EventType type, UINT32 idx)
                : Type(type)
                , Idx(idx)
            { }

            EventType Type;
            UINT32 Idx;
        };

    public:
        Input();
        ~Input();

        TE_MODULE_STATIC_HEADER_MEMBER(Input)

        void Update();
        void TriggerCallbacks();

        /** Returns internal, platform specific privata data. */
        InputPrivateData* GetPrivateData() const { return _platformData; }

        /** Returns a handle to the window that is currently receiving input. */
        UINT64 GetWindowHandle() const { return _windowHandle; }

        /** Returns position of the pointer (for example mouse cursor) relative to the screen. */
        Vector2I GetPointerPosition() const;

        /** Returns difference between pointer position between current and last frame. */
        Vector2I GetPointerDelta() const { return _pointerDelta; }

        /** Returns the number of detected devices of the specified type. */
        UINT32 GetDeviceCount(InputDevice device) const;

        /**
         * Returns value of the specified input axis. Normally in range [-1.0, 1.0] but can be outside the range for
         * devices with unbound axes (for example mouse).
         *
         * @param[in]	type		Type of axis to query. Usually a type from InputAxis but can be a custom value.
         * @param[in]	deviceIdx	Index of the device in case more than one is hooked up (0 - primary).
         */
        float GetAxisValue(UINT32 type, UINT32 deviceIdx = 0) const;

        /**
         * Query if the provided button is currently being held (this frame or previous frames).
         *
         * @param[in]	keyCode		Code of the button to query.
         * @param[in]	deviceIdx	Device to query the button on (0 - primary).
         */
        bool IsButtonHeld(ButtonCode button, UINT32 deviceIdx = 0) const;

        /**
         * Query if the provided button is currently being released (only true for one frame).
         *
         * @param[in]	keyCode		Code of the button to query.
         * @param[in]	deviceIdx	Device to query the button on (0 - primary).
         */
        bool IsButtonUp(ButtonCode button, UINT32 deviceIdx = 0) const;

        /**
         * Query if the provided button is currently being pressed (only true for one frame).
         *
         * @param[in]	keyCode		Code of the button to query.
         * @param[in]	deviceIdx	Device to query the button on (0 - primary).
         */
        bool IsButtonDown(ButtonCode button, UINT32 deviceIdx = 0) const;

        /**
         * Query if the provided pointer button is currently being held (this frame or previous frames).
         * @param[in]	pointerButton		Code of the button to query.
         */
        bool IsPointerButtonHeld(PointerEventButton pointerButton) const;

        /**
         * Query if the provided pointer button is currently being released (only true for one frame).
         * @param[in]	pointerButton		Code of the button to query.
         */
        bool IsPointerButtonUp(PointerEventButton pointerButton) const;

        /**
         * Query if the provided pointer button is currently being pressed (only true for one frame).
         *
         * @param[in]	pointerButton		Code of the button to query.
         */
        bool IsPointerButtonDown(PointerEventButton pointerButton) const;

        /** Query has the left pointer button has been double-clicked this frame. */
        bool IsPointerDoubleClicked() const;

    public:
        /** Called by Mouse when mouse movement is detected. */
        void NotifyMouseMoved(INT32 relX, INT32 relY, INT32 relZ);

        /** Called by any of the raw input devices when analog axis movement is detected. */
        void NotifyAxisMoved(UINT32 gamepadIdx, UINT32 axisIdx, INT32 value);

        /** Called by any of the raw input devices when a button is pressed. */
        void NotifyButtonPressed(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp);

        /** Called by any of the raw input devices when a button is released. */
        void NotifyButtonReleased(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp);

    public:
        /**
         * Called from the message loop to notify user has entered a character.
         */
        void CharInput(UINT32 character);

        /**
         * Called from the message loop to notify user has moved the cursor.
         */
        void CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates);

        /**
         * Called from the message loop to notify user has pressed a mouse button.
         */
        void CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates);

        /**
         * Called from the message loop to notify user has released a mouse button.
         */
        void CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates);

        /**
         * Called from the message loop to notify user has double-clicked a mouse button.
         */
        void CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates);

        /**
         * Called from the message loop to notify user has scrolled the mouse wheel.
         */
        void MouseWheelScrolled(float scrollPos);

        /** Called when window in focus changes, as reported by the OS. */
        void InputWindowChanged(RenderWindow& win);

        /**
         * Called when the current window loses input focus. This might be followed by inputWindowChanged() if the focus
         * just switched to another of this application's windows.
         */
        void InputFocusLost();

    public:
        /** Triggered whenever a button is first pressed. */
        Event<void(const ButtonEvent&)> OnButtonDown;

        /**	Triggered whenever a button is first released. */
        Event<void(const ButtonEvent&)> OnButtonUp;

        /**	Triggered whenever user inputs a text character. */
        Event<void(const TextInputEvent&)> OnCharInput;

        /**	Triggers when some pointing device (mouse cursor, touch) moves. */
        Event<void(const PointerEvent&)> OnPointerMoved;

        /**	Triggers when some pointing device (mouse cursor, touch) button is pressed. */
        Event<void(const PointerEvent&)> OnPointerPressed;

        /**	Triggers when some pointing device (mouse cursor, touch) button is released. */
        Event<void(const PointerEvent&)> OnPointerReleased;

        /**	Triggers when some pointing device (mouse cursor, touch) button is double clicked. */
        Event<void(const PointerEvent&)> OnPointerDoubleClick;

    protected:
        /** Performs platform specific raw input system initialization. */
        void InitRawInput();

        /** Performs platform specific raw input system cleanup. */
        void CleanUpRawInput();

        /**	Triggered by input handler when a button is pressed. */
        void ButtonDown(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp);

        /**	Triggered by input handler when a button is released. */
        void ButtonUp(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp);

        /**	Triggered by input handler when a mouse/joystick axis is moved. */
        void AxisMoved(UINT32 deviceIdx, float value, UINT32 axis);

    protected:
        Mouse* _mouse;
        Keyboard* _keyboard;
        Vector<GamePad*> _gamepads;

        // OS input events
        HEvent _charInputConn;
        HEvent _cursorMovedConn;
        HEvent _cursorPressedConn;
        HEvent _cursorReleasedConn;
        HEvent _cursorDoubleClickConn;
        HEvent _inputCommandConn;
        HEvent _mouseWheelScrolledConn;

        Vector<DeviceData> _devices;
        Vector2I _pointerPosition;
        Vector2I _lastPointerPosition;
        Vector2I _pointerDelta;
        ButtonState _pointerButtonStates[3];
        OSPointerButtonStates _pointerState;
        float _mouseScroll;

        bool _pointerDoubleClicked;
        bool _lastPositionSet;

        Vector<QueuedEvent> _queuedEvents[2];

        Vector<TextInputEvent> _textInputEvents[2];
        Vector<PointerEvent> _pointerDoubleClickEvents[2];
        Vector<PointerEvent> _pointerReleasedEvents[2];
        Vector<PointerEvent> _pointerPressedEvents[2];

        Vector<ButtonEvent> _buttonDownEvents[2];
        Vector<ButtonEvent> _buttonUpEvents[2];

        float _totalMouseSamplingTime[2];
        UINT32 _totalMouseNumSamples[2];
        float _mouseZeroTime[2];
        INT32 _mouseSampleAccumulator[2];
        float _mouseSmoothedAxis[2];
        UINT64 _lastMouseUpdateFrame;

        // Raw input
        UINT64 _windowHandle;
        InputPrivateData* _platformData;

        Mutex _mutex;
    };

    /**	Provides easy access to Input. */
    TE_CORE_EXPORT Input& gInput();
    TE_CORE_EXPORT Input* gInputPtr();
}