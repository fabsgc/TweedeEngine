#pragma once

#include "Platform/TePlatform.h"

namespace te
{
    /** Various Win32 specific functionality. Contains the main message loop. */
    class TE_CORE_EXPORT Win32Platform : public Platform
    {
    public:
        /** Main message loop callback that processes messages received from windows. */
        static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
}
