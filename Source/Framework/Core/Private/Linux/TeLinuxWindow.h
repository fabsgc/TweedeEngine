#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeVector2I.h"

namespace te
{
    /**	Descriptor used for creating a platform specific native window. */
	struct WINDOW_DESC
	{
    };

    /**
	 * Represents a X11 window. Note that all accesses (including creation and destruction) to objects of this class must
	 * be locked by the main X11 lock accessible through LinuxPlatform.
	 */
	class TE_UTILITY_EXPORT LinuxWindow
	{
	public:
		LinuxWindow(const WINDOW_DESC& desc);
		~LinuxWindow();

    private:
        struct Pimpl;
		Pimpl* _data;
    };
}