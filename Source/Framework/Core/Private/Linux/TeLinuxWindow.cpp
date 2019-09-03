#include "Private/Linux/TeLinuxWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    struct LinuxWindow::Pimpl
	{
		// TODO
    };

    LinuxWindow::LinuxWindow(const WINDOW_DESC &desc)
	{
		_data = te_new<Pimpl>(); // TODO
    }

    LinuxWindow::~LinuxWindow()
	{
		te_delete(_data); // TODO
	}

	void LinuxWindow::Move(INT32 x, INT32 y)
	{
		// TODO
	}

	void LinuxWindow::Resize(UINT32 width, UINT32 height)
	{
		// TODO
	}

	void LinuxWindow::Hide()
	{
		// TODO
	}

	void LinuxWindow::Show()
	{
		// TODO
	}

	void LinuxWindow::Maximize()
	{
		// TODO
	}

	void LinuxWindow::Minimize()
	{
		// TODO
	}

	void LinuxWindow::Restore()
	{
		// TODO
	}

	INT32 LinuxWindow::GetLeft() const
	{
		return 0; // TODO
	}

	INT32 LinuxWindow::GetTop() const
	{
		return 0; // TODO
	}

	UINT32 LinuxWindow::GetWidth() const
	{
		return 0; // TODO
	}

	UINT32 LinuxWindow::GetHeight() const
	{
		return 0; // TODO
	}

	Vector2I LinuxWindow::WindowToScreenPos(const Vector2I& windowPos) const
	{
		return Vector2I(); // TODO
	}

	Vector2I LinuxWindow::ScreenToWindowPos(const Vector2I& screenPos) const
	{
		return Vector2I(); // TODO
	}
}