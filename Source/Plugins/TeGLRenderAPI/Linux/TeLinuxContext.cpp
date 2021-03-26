#include "Linux/TeLinuxContext.h"
#include "Private/Linux/TeLinuxPlatform.h"
#include "Linux/TeLinuxGLSupport.h"

namespace te
{ 
    typedef int(*ErrorHandlerProc)(::Display*, XErrorEvent*);
    int ContextErrorHandler(::Display* display, XErrorEvent* error)
    {
        // Do nothing
        return 0;
    }

    LinuxContext::LinuxContext(::Display* display, XVisualInfo& visualInfo)
    : _display(display)
    {
        LinuxPlatform::LockX();

        LinuxPlatform::UnlockX();
    }

    LinuxContext::~LinuxContext()
    {
        ReleaseContext();
    }

    void LinuxContext::SetCurrent(const RenderWindow& window)
    {
        window.GetCustomAttribute("WINDOW", &_currentWindow);

        LinuxPlatform::LockX();

        LinuxPlatform::UnlockX();
    }

    void LinuxContext::EndCurrent()
	{
		LinuxPlatform::LockX();

		LinuxPlatform::UnlockX();
	}

	void LinuxContext::releaseContext()
	{
		if (_context)
		{
			LinuxPlatform::LockX();

			LinuxPlatform::UnlockX();
		}
	}
}
