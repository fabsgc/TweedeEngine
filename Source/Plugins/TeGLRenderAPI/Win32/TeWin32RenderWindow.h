#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderWindow.h"
#include "Private/Win32/TeWin32Window.h"
#include "Math/TeVector2I.h"

namespace te
{
    class Win32RenderWindow : public RenderWindow
    {
    public:
        Win32RenderWindow(const RENDER_WINDOW_DESC& desc);
        ~Win32RenderWindow();

        void Initialize() override;
        void InitializeGui() override;
        void GetCustomAttribute(const String& name, void* pData) const override;
        void WindowMovedOrResized() override;

        /** Retrieves internal window handle. */
        HWND GetHWnd() const;

        /** @copydoc RenderWindow::move */
        void Move(INT32 left, INT32 top) override;

        /** @copydoc RenderWindow::resize */
        void Resize(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::setHidden */
        void SetHidden(bool hidden) override;

        /** @copydoc RenderWindow::setActive */
        void SetActive(bool state) override;

        /** @copydoc RenderWindow::minimize */
        void Minimize() override;

        /** @copydoc RenderWindow::maximize */
        void Maximize() override;

        /** @copydoc RenderWindow::restore */
        void Restore() override;

        /** @copydoc RenderWindow::setFullscreen(UINT32, UINT32, float, UINT32) */
        void SetFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) override;

        /** @copydoc RenderWindow::setFullscreen(const VideoMode&) */
        void SetFullscreen(const VideoMode& videoMode) override;

        /** @copydoc RenderWindow::setWindowed */
        void SetWindowed(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::screenToWindowPos */
        Vector2I ScreenToWindowPos(const Vector2I& screenPos) const override;

        /** @copydoc RenderWindow::windowToScreenPos */
        Vector2I WindowToScreenPos(const Vector2I& windowPos) const override;

        /** @copydoc RenderWindow::SetTitle */
        void SetTitle(const String& title) override;

    protected:
        Win32Window* _window;
        char* _deviceName;
        int _displayFrequency;
        HDC	_HDC;
    };
}
