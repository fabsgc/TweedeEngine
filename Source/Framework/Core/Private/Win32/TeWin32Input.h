#pragma once

#include "TeCorePrerequisites.h"

#ifndef DIRECTINPUT_VERSION
#   define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#include <Xinput.h>

#include <wbemidl.h>
#include <oleauto.h>

namespace te
{
    /** Information about a gamepad from DirectInput. */
    struct GamePadInfo
    {
        UINT32 Id;
        GUID GuidInstance;
        GUID GuidProduct;
        String Name;

        bool IsXInput;
        int XInputDev;
    };

    /**
     * Data specific to Win32 implementation of the input system. Can be passed to platform specific implementations of
     * the individual device types.
     */
    struct InputPrivateData
    {
        IDirectInput8* DirectInput;
        Vector<GamePadInfo> GamepadInfos;

        DWORD KbSettings;
        DWORD MouseSettings;
    };

    // Max number of elements to collect from buffered DirectInput
    #define DI_BUFFER_SIZE_KEYBOARD 17
    #define DI_BUFFER_SIZE_MOUSE 128
    #define DI_BUFFER_SIZE_GAMEPAD 129
}