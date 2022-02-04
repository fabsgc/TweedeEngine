#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderWindow.h"
#include "Math/TeVector2I.h"
#include "Linux/TeLinuxGLSupport.h"
#include "Linux/TeLinuxContext.h"

#include <X11/Xutil.h>

namespace te
{
    class LinuxWindow;

    class LinuxRenderWindow : public RenderWindow
    {
    public:
        LinuxRenderWindow(const RENDER_WINDOW_DESC& desc, LinuxGLSupport& glsupport);
        virtual ~LinuxRenderWindow();

        void Initialize() override;
        void InitializeGui() override;
        void GetCustomAttribute(const String& name, void* pData) const override;
        void WindowMovedOrResized() override;

        /** @copydoc RenderWindow::Move */
        void Move(INT32 left, INT32 top) override;

        /** @copydoc RenderWindow::Resize */
        void Resize(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::SetHidden */
        void SetHidden(bool hidden) override;

        /** @copydoc RenderWindow::SetActive */
        void SetActive(bool state) override;

        /** @copydoc RenderWindow::Minimize */
        void Minimize() override;

        /** @copydoc RenderWindow::Maximize */
        void Maximize() override;

        /** @copydoc RenderWindow::Restore */
        void Restore() override;

        /** @copydoc RenderWindow::SetVSync */
        void SetVSync(bool enabled) override;

        /** @copydoc RenderWindow::SetFullscreen(UINT32, UINT32, float, UINT32) */
        void SetFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) override;

        /** @copydoc RenderWindow::SetFullscreen(const VideoMode&) */
        void SetFullscreen(const VideoMode& videoMode) override;

        /** @copydoc RenderWindow::SetWindowed */
        void SetWindowed(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::ScreenToWindowPos */
        Vector2I ScreenToWindowPos(const Vector2I& screenPos) const override;

        /** @copydoc RenderWindow::WindowToScreenPos */
        Vector2I WindowToScreenPos(const Vector2I& windowPos) const override;

        /** @copydoc RenderWindow::SetTitle */
        void SetTitle(const String& title) override;

    protected:
        LinuxWindow* _window;
        LinuxGLSupport& _GLSupport;
        SPtr<LinuxContext> _context;
    };
}
