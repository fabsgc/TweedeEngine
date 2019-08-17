#include "Private/Linux/TeLinuxWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    struct LinuxWindow::Pimpl
	{
    };

    LinuxWindow::LinuxWindow(const WINDOW_DESC &desc)
	{
		_data = te_new<Pimpl>();
    }

    LinuxWindow::~LinuxWindow()
	{
		te_delete(_data);
	}
}