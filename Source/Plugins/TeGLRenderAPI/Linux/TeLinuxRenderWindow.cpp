#include "TeLinuxRenderWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    LinuxRenderWindow::LinuxRenderWindow(const RENDER_WINDOW_DESC& desc)
        : RenderWindow(desc)
    {
    }

    void LinuxRenderWindow::Update()
    {
        //TODO
    }

    void LinuxRenderWindow::Initialize()
    {
        //TODO
    }

    void LinuxRenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        RenderWindow::GetCustomAttribute(name, pData);
    }

    void LinuxRenderWindow::Move(INT32 left, INT32 top)
    {
        //TODO
    }

    void LinuxRenderWindow::Resize(UINT32 width, UINT32 height)
    {
        //TODO
    }

    void LinuxRenderWindow::SetHidden(bool hidden)
    {
        //TODO
    }

    void LinuxRenderWindow::SetActive(bool state)
    {
        //TODO
    }

    void LinuxRenderWindow::Minimize()
    {
        //TODO
    }

    void LinuxRenderWindow::Maximize()
    {
        //TODO
    }

    void LinuxRenderWindow::Restore()
    {
        //TODO
    }

    void LinuxRenderWindow::SetFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
    {
        //TODO
    }

    void LinuxRenderWindow::SetFullscreen(const VideoMode& videoMode)
    {
        //TODO
    }

    void LinuxRenderWindow::SetWindowed(UINT32 width, UINT32 height)
    {
        //TODO
    }

    Vector2I LinuxRenderWindow::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        //TODO

        return Vector2I();
    }

    Vector2I LinuxRenderWindow::WindowToScreenPos(const Vector2I& windowPos) const
    {
        //TODO

        return Vector2I();
    }

    void LinuxRenderWindow::WindowMovedOrResized()
    {
        //TODO
    }
}