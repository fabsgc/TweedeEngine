#pragma once

#include "Platform/TePlatform.h"
#include "Private/Linux/TeLinuxWindow.h"
#include "Private/Linux/TeLinuxInput.h"
#include <X11/X.h>
#include <X11/Xlib.h>

namespace te
{
    /**
     * Contains various Linux specific platform functionality;
     */
    class TE_CORE_EXPORT LinuxPlatform : public Platform
    {
    public:
        /** Returns the active X11 display. */
        static ::Display* GetXDisplay();

        /** Returns the main X11 window. Caller must ensure the main window has been created. */
        static ::Window GetMainXWindow();

        /** Locks access to the X11 system, not allowing any other thread to access it. Must be used for every X11 access. */
        static void LockX();

        /** Unlocks access to the X11 system. Must follow every call to lockX(). */
        static void UnlockX();

        /** Notifies the system that a new window was created. */
        static void RegisterWindow(::Window xWindow, LinuxWindow* window);

    public:
        /**
         * Stores events captured on the core thread, waiting to be processed by the main thread.
         * Always lock on eventLock when accessing this.
         */
        static Queue<LinuxButtonEvent> ButtonEvents;

        /**
         * Stores accumulated mouse motion events, waiting to be processed by the main thread.
         * Always lock on eventLock when accessing this.
         */
        static LinuxMouseMotionEvent MouseMotionEvent;
    };
}
