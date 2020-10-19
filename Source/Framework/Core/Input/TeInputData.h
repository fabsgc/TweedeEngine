#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector2I.h"

namespace te
{

//Conflicts with other libs
#undef None

    /**
     * Contains all possible input buttons, including keyboard scan codes, mouse buttons and gamepad buttons.
     *
     * @note
     * These codes are only keyboard scan codes. This means that exact scan code identifier might not correspond to that 
     * exact character on user's keyboard, depending on user's input locale. Only for US locale will these scan code names 
     * match the actual keyboard input. Think of the US key code names as only a convenience for more easily identifying
     * which location on the keyboard a scan code represents.
     * @note
     * When storing these sequentially make sure to only reference the low order 2 bytes. Two high order bytes are used for 
     * various flags.
     */
    enum ButtonCode : UINT32
    {
        TE_UNASSIGNED  = 0x00,
        TE_ESCAPE      = 0x01,
        TE_1           = 0x02,
        TE_2           = 0x03,
        TE_3           = 0x04,
        TE_4           = 0x05,
        TE_5           = 0x06,
        TE_6           = 0x07,
        TE_7           = 0x08,
        TE_8           = 0x09,
        TE_9           = 0x0A,
        TE_0           = 0x0B,
        TE_MINUS       = 0x0C,    // - on main keyboard
        TE_EQUALS      = 0x0D,
        TE_BACK        = 0x0E,    // backspace
        TE_TAB         = 0x0F,
        TE_Q           = 0x10,
        TE_W           = 0x11,
        TE_E           = 0x12,
        TE_R           = 0x13,
        TE_T           = 0x14,
        TE_Y           = 0x15,
        TE_U           = 0x16,
        TE_I           = 0x17,
        TE_O           = 0x18,
        TE_P           = 0x19,
        TE_LBRACKET    = 0x1A,
        TE_RBRACKET    = 0x1B,
        TE_RETURN      = 0x1C,    // Enter on main keyboard
        TE_LCONTROL    = 0x1D,
        TE_A           = 0x1E,
        TE_S           = 0x1F,
        TE_D           = 0x20,
        TE_F           = 0x21,
        TE_G           = 0x22,
        TE_H           = 0x23,
        TE_J           = 0x24,
        TE_K           = 0x25,
        TE_L           = 0x26,
        TE_SEMICOLON   = 0x27,
        TE_APOSTROPHE  = 0x28,
        TE_GRAVE       = 0x29,    // accent
        TE_LSHIFT      = 0x2A,
        TE_BACKSLASH   = 0x2B,
        TE_Z           = 0x2C,
        TE_X           = 0x2D,
        TE_C           = 0x2E,
        TE_V           = 0x2F,
        TE_B           = 0x30,
        TE_N           = 0x31,
        TE_M           = 0x32,
        TE_COMMA       = 0x33,
        TE_PERIOD      = 0x34,    // . on main keyboard
        TE_SLASH       = 0x35,    // / on main keyboard
        TE_RSHIFT      = 0x36,
        TE_MULTIPLY    = 0x37,    // * on numeric keypad
        TE_LMENU       = 0x38,    // left Alt
        TE_SPACE       = 0x39,
        TE_CAPITAL     = 0x3A,
        TE_F1          = 0x3B,
        TE_F2          = 0x3C,
        TE_F3          = 0x3D,
        TE_F4          = 0x3E,
        TE_F5          = 0x3F,
        TE_F6          = 0x40,
        TE_F7          = 0x41,
        TE_F8          = 0x42,
        TE_F9          = 0x43,
        TE_F10         = 0x44,
        TE_NUMLOCK     = 0x45,
        TE_SCROLL      = 0x46,    // Scroll Lock
        TE_NUMPAD7     = 0x47,
        TE_NUMPAD8     = 0x48,
        TE_NUMPAD9     = 0x49,
        TE_SUBTRACT    = 0x4A,    // - on numeric keypad
        TE_NUMPAD4     = 0x4B,
        TE_NUMPAD5     = 0x4C,
        TE_NUMPAD6     = 0x4D,
        TE_ADD         = 0x4E,    // + on numeric keypad
        TE_NUMPAD1     = 0x4F,
        TE_NUMPAD2     = 0x50,
        TE_NUMPAD3     = 0x51,
        TE_NUMPAD0     = 0x52,
        TE_DECIMAL     = 0x53,    // . on numeric keypad
        TE_OEM_102     = 0x56,    // < > | on UK/Germany keyboards
        TE_F11         = 0x57,
        TE_F12         = 0x58,
        TE_F13         = 0x64,    //                     (NEC PC98)
        TE_F14         = 0x65,    //                     (NEC PC98)
        TE_F15         = 0x66,    //                     (NEC PC98)
        TE_KANA        = 0x70,    // (Japanese keyboard)
        TE_ABNT_C1     = 0x73,    // / ? on Portugese (Brazilian) keyboards
        TE_CONVERT     = 0x79,    // (Japanese keyboard)
        TE_NOCONVERT   = 0x7B,    // (Japanese keyboard)
        TE_YEN         = 0x7D,    // (Japanese keyboard)
        TE_ABNT_C2     = 0x7E,    // Numpad . on Portugese (Brazilian) keyboards
        TE_NUMPADEQUALS= 0x8D,    // = on numeric keypad (NEC PC98)
        TE_PREVTRACK   = 0x90,    // Previous Track (TE_CIRCUMFLEX on Japanese keyboard)
        TE_AT          = 0x91,    //                     (NEC PC98)
        TE_COLON       = 0x92,    //                     (NEC PC98)
        TE_UNDERLINE   = 0x93,    //                     (NEC PC98)
        TE_KANJI       = 0x94,    // (Japanese keyboard)
        TE_STOP        = 0x95,    //                     (NEC PC98)
        TE_AX          = 0x96,    //                     (Japan AX)
        TE_UNLABELED   = 0x97,    //                        (J3100)
        TE_NEXTTRACK   = 0x99,    // Next Track
        TE_NUMPADENTER = 0x9C,    // Enter on numeric keypad
        TE_RCONTROL    = 0x9D,
        TE_MUTE        = 0xA0,    // Mute
        TE_CALCULATOR  = 0xA1,    // Calculator
        TE_PLAYPAUSE   = 0xA2,    // Play / Pause
        TE_MEDIASTOP   = 0xA4,    // Media Stop
        TE_VOLUMEDOWN  = 0xAE,    // Volume -
        TE_VOLUMEUP    = 0xB0,    // Volume +
        TE_WEBHOME     = 0xB2,    // Web home
        TE_NUMPADCOMMA = 0xB3,    // , on numeric keypad (NEC PC98)
        TE_DIVIDE      = 0xB5,    // / on numeric keypad
        TE_SYSRQ       = 0xB7,
        TE_RMENU       = 0xB8,    // right Alt
        TE_PAUSE       = 0xC5,    // Pause
        TE_HOME        = 0xC7,    // Home on arrow keypad
        TE_UP          = 0xC8,    // UpArrow on arrow keypad
        TE_PGUP        = 0xC9,    // PgUp on arrow keypad
        TE_LEFT        = 0xCB,    // LeftArrow on arrow keypad
        TE_RIGHT       = 0xCD,    // RightArrow on arrow keypad
        TE_END         = 0xCF,    // End on arrow keypad
        TE_DOWN        = 0xD0,    // DownArrow on arrow keypad
        TE_PGDOWN      = 0xD1,    // PgDn on arrow keypad
        TE_INSERT      = 0xD2,    // Insert on arrow keypad
        TE_DELETE      = 0xD3,    // Delete on arrow keypad
        TE_LWIN        = 0xDB,    // Left Windows key
        TE_RWIN        = 0xDC,    // Right Windows key
        TE_APPS        = 0xDD,    // AppMenu key
        TE_POWER       = 0xDE,    // System Power
        TE_SLEEP       = 0xDF,    // System Sleep
        TE_WAKE        = 0xE3,    // System Wake
        TE_WEBSEARCH   = 0xE5,    // Web Search
        TE_WEBFAVORITES= 0xE6,    // Web Favorites
        TE_WEBREFRESH  = 0xE7,    // Web Refresh
        TE_WEBSTOP     = 0xE8,    // Web Stop
        TE_WEBFORWARD  = 0xE9,    // Web Forward
        TE_WEBBACK     = 0xEA,    // Web Back
        TE_MYCOMPUTER  = 0xEB,    // My Computer
        TE_MAIL        = 0xEC,    // Mail
        TE_MEDIASELECT = 0xED,    // Media Select
        TE_MOUSE_LEFT = 0x800000EE, // Mouse buttons - Most important bit signifies this key is a mouse button
        TE_MOUSE_RIGHT = 0x800000EF,
        TE_MOUSE_MIDDLE = 0x800000F0,
        TE_MOUSE_BTN4 = 0x800000F1,
        TE_MOUSE_BTN5 = 0x800000F2,
        TE_MOUSE_BTN6 = 0x800000F3,
        TE_MOUSE_BTN7 = 0x800000F4,
        TE_MOUSE_BTN8 = 0x800000F5,
        TE_MOUSE_BTN9 = 0x800000F6,
        TE_MOUSE_BTN10 = 0x800000F7,
        TE_MOUSE_BTN11 = 0x800000F8,
        TE_MOUSE_BTN12 = 0x800000F9,
        TE_MOUSE_BTN13 = 0x800000FA,
        TE_MOUSE_BTN14 = 0x800000FB,
        TE_MOUSE_BTN15 = 0x800000FC,
        TE_MOUSE_BTN16 = 0x800000FD,
        TE_MOUSE_BTN17,
        TE_MOUSE_BTN18,
        TE_MOUSE_BTN19,
        TE_MOUSE_BTN20,
        TE_MOUSE_BTN21,
        TE_MOUSE_BTN22,
        TE_MOUSE_BTN23,
        TE_MOUSE_BTN24,
        TE_MOUSE_BTN25,
        TE_MOUSE_BTN26,
        TE_MOUSE_BTN27,
        TE_MOUSE_BTN28,
        TE_MOUSE_BTN29,
        TE_MOUSE_BTN30,
        TE_MOUSE_BTN31,
        TE_MOUSE_BTN32,
        TE_GAMEPAD_A = 0x4000010F, // Joystick/Gamepad buttons- Second most important bit signifies key is a gamepad button
        TE_GAMEPAD_B = 0x40000110, // Similar to keyboard names, these are for convenience named after Xbox controller buttons
        TE_GAMEPAD_X = 0x40000111, // but if some other controller is connected you will need to learn yourself which of these
        TE_GAMEPAD_Y = 0x40000112, // corresponds to which actual button on the controller.
        TE_GAMEPAD_LB = 0x40000113,
        TE_GAMEPAD_RB = 0x40000114,
        TE_GAMEPAD_LS = 0x40000115,
        TE_GAMEPAD_RS = 0x40000116,
        TE_GAMEPAD_BACK = 0x40000117,
        TE_GAMEPAD_START = 0x40000118,
        TE_GAMEPAD_DPAD_LEFT = 0x40000119,
        TE_GAMEPAD_DPAD_RIGHT = 0x4000011A,
        TE_GAMEPAD_DPAD_UP = 0x4000011B,
        TE_GAMEPAD_DPAD_DOWN = 0x4000011C,
        TE_GAMEPAD_BTN1 = 0x4000011D,
        TE_GAMEPAD_BTN2 = 0x4000011E,
        TE_GAMEPAD_BTN3 = 0x4000011F,
        TE_GAMEPAD_BTN4 = 0x40000120,
        TE_GAMEPAD_BTN5 = 0x40000121,
        TE_GAMEPAD_BTN6 = 0x40000122,
        TE_GAMEPAD_BTN7 = 0x40000123,
        TE_GAMEPAD_BTN8 = 0x40000124,
        TE_GAMEPAD_BTN9 = 0x40000125,
        TE_GAMEPAD_BTN10 = 0x40000126,
        TE_GAMEPAD_BTN11 = 0x40000127,
        TE_GAMEPAD_BTN12 = 0x40000128,
        TE_GAMEPAD_BTN13 = 0x40000129,
        TE_GAMEPAD_BTN14 = 0x4000012A,
        TE_GAMEPAD_BTN15 = 0x4000012B,
        TE_GAMEPAD_BTN16 = 0x4000012C,
        TE_GAMEPAD_BTN17,
        TE_GAMEPAD_BTN18,
        TE_GAMEPAD_BTN19,
        TE_GAMEPAD_BTN20,
        TE_GAMEPAD_DPAD_UPLEFT,
        TE_GAMEPAD_DPAD_UPRIGHT,
        TE_GAMEPAD_DPAD_DOWNLEFT,
        TE_GAMEPAD_DPAD_DOWNRIGHT,
        TE_NUM_KEYS = TE_MEDIASELECT - TE_UNASSIGNED + 1, // IMPORTANT: Make sure to update these if you modify the values above
        TE_NUM_MOUSE = TE_MOUSE_BTN32 - TE_MOUSE_LEFT + 1,
        TE_NUM_GAMEPAD = TE_GAMEPAD_DPAD_DOWNRIGHT - TE_GAMEPAD_A + 1,
        TE_COUNT = TE_NUM_KEYS + TE_NUM_MOUSE + TE_NUM_GAMEPAD,
    };

    /** Types of input devices. */
    enum class InputDevice
    {
        Keyboard,
        Mouse,
        Gamepad,
        Count // Keep at end
    };

    /** Contains data about a button input event. */
    struct ButtonEvent
    {
    public:
        ButtonEvent()
            :_isUsed(false)
        { }

        ButtonCode buttonCode; /**< Button code this event is referring to. */
        UINT64 timestamp; /**< Timestamp in ticks when the event happened. */
        UINT32 deviceIdx; /**< Index of the device that the event originated from. */

        /** Query is the pressed button a keyboard button. */
        bool isKeyboard() const { return (buttonCode & 0xC0000000) == 0; }

        /** Query is the pressed button a mouse button. */
        bool isMouse() const { return (buttonCode & 0x80000000) != 0; }

        /** Query is the pressed button a gamepad button. */
        bool isGamepad() const { return (buttonCode & 0x40000000) != 0; }

        /**
         * Check if the event has been marked as used. Internally this means nothing but caller might choose to ignore an
         * used event.
         */
        bool isUsed() const { return _isUsed; }

        /** Mark the event as used. Internally this means nothing but caller might choose to ignore an used event. */
        void markAsUsed() const { _isUsed = true; }
    private:
        mutable bool _isUsed;
    };

    /**
     * Pointer buttons. Generally these correspond to mouse buttons, but may be used in some form for touch input as well.
     */
    enum class PointerEventButton
    {
        Left, Middle, Right, Count
    };

    /** Type of pointer event.*/
    enum class PointerEventType
    {
        CursorMoved,
        ButtonPressed,
        ButtonReleased,
        DoubleClick
    };

    /**
     * Event that gets sent out when user interacts with the screen in some way, usually by moving the mouse cursor or
     * using touch input.
     */
    struct PointerEvent
    {
    public:
        PointerEvent()
            : button(PointerEventButton::Left), type(PointerEventType::CursorMoved), shift(false)
            , control(false), alt(false), mouseWheelScrollAmount(0.0f), _isUsed(false)
        {
            buttonStates[0] = false;
            buttonStates[1] = false;
            buttonStates[2] = false;
        }

        Vector2I screenPos; /**< Screen position where the input event occurred. */
        Vector2I delta; /**< Change in movement since last sent event. */
        /** States of the pointer buttons (for example mouse buttons). */
        bool buttonStates[(UINT32)PointerEventButton::Count];
        /**
         * Button that triggered the pointer event. Might be irrelevant depending on event type. (for example move events
         * don't correspond to a button.
         */
        PointerEventButton button;
        PointerEventType type; /**< Type of the pointer event. */

        bool shift; /**< Is shift button on the keyboard being held down. */
        bool control; /**< Is control button on the keyboard being held down. */
        bool alt; /**< Is alt button on the keyboard being held down. */

        /** If mouse wheel is being scrolled, what is the amount. Only relevant for move events. */
        float mouseWheelScrollAmount;

        /**
         * Check if the event has been marked as used. Internally this means nothing but caller might choose to ignore an
         * used event.
         */
        bool isUsed() const { return _isUsed; }

        /** Mark the event as used. Internally this means nothing but caller might choose to ignore an used event. */
        void markAsUsed() const { _isUsed = true; }

    private:
        mutable bool _isUsed;
    };

    /**	Types of special input commands. */
    enum class InputCommandType
    {
        CursorMoveLeft, CursorMoveRight, CursorMoveUp, CursorMoveDown,
        SelectLeft, SelectRight, SelectUp, SelectDown,
        Escape, Delete, Backspace, Return, Confirm, Tab
    };

    /**
     * Event that gets sent out when user inputs some text. These events may be preceeded by normal button events if user
     * is typing on a keyboard.
     */
    struct TextInputEvent
    {
    public:
        TextInputEvent()
            :_isUsed(false)
        { }

        UINT32 textChar; /**< Character the that was input. */

        /**
         * Check if the event has been marked as used. Internally this means nothing but caller might choose to ignore an
         * used event.
         */
        bool isUsed() const { return _isUsed; }

        /** Mark the event as used. Internally this means nothing but caller might choose to ignore an used event. */
        void markAsUsed() const { _isUsed = true; }

    private:
        mutable bool _isUsed;
    };

    /** Common input axis types. */
    enum class InputAxis
    {
        MouseX, /**< Mouse axis X. Provides unnormalized relative movement. */
        MouseY, /**< Mouse axis Y. Provides unnormalized relative movement. */
        MouseZ, /**< Mouse wheel/scroll axis. Provides unnormalized relative movement. */
        LeftStickX, /**< Gamepad left stick X. Provides normalized ([-1, 1] range) absolute position. */
        LeftStickY, /**<  Gamepad left stick Y. Provides normalized ([-1, 1] range) absolute position. */
        RightStickX, /**< Gamepad right stick X. Provides normalized ([-1, 1] range) absolute position.*/
        RightStickY, /**< Gamepad right stick Y. Provides normalized ([-1, 1] range) absolute position. */
        LeftTrigger, /**< Gamepad left trigger. Provides normalized ([-1, 1] range) absolute position. */
        RightTrigger, /**< Gamepad right trigger. Provides normalized ([-1, 1] range) absolute position. */
        Count // Keep at end
    };

    /** Modifiers used with along with keyboard buttons. */
    enum class ButtonModifier
    {
        None = 0x00,
        Shift = 0x01,
        Ctrl = 0x02,
        Alt = 0x04,
        ShiftCtrl = 0x03,
        CtrlAlt = 0x06,
        ShiftAlt = 0x05,
        ShiftCtrlAlt = 0x07
    };
}
