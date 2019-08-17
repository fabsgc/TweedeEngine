#include "TeGLRenderWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    GLRenderWindow::GLRenderWindow(const RENDER_WINDOW_DESC& desc)
        : RenderWindow(desc)
    {
    }

    void GLRenderWindow::Update()
    {
        //TODO
    }

    void GLRenderWindow::Initialize()
    {
        //TODO
    }

    void GLRenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        RenderWindow::GetCustomAttribute(name, pData);
    }

    void GLRenderWindow::Move(INT32 left, INT32 top)
    {
        //TODO
    }

    void GLRenderWindow::Resize(UINT32 width, UINT32 height)
    {
        //TODO
    }

    void GLRenderWindow::SetHidden(bool hidden)
    {
        //TODO
    }

    void GLRenderWindow::SetActive(bool state)
    {
        //TODO
    }

    void GLRenderWindow::Minimize()
    {
        //TODO
    }

    void GLRenderWindow::Maximize()
    {
        //TODO
    }

    void GLRenderWindow::Restore()
    {
        //TODO
    }

    void GLRenderWindow::SetFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
    {
        //TODO
    }

    void GLRenderWindow::SetFullscreen(const VideoMode& videoMode)
    {
        //TODO
    }

    void GLRenderWindow::SetWindowed(UINT32 width, UINT32 height)
    {
        //TODO
    }

    Vector2I GLRenderWindow::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        //TODO

        return Vector2I();
    }

    Vector2I GLRenderWindow::WindowToScreenPos(const Vector2I& windowPos) const
    {
        //TODO

        return Vector2I();
    }

    void GLRenderWindow::WindowMovedOrResized()
    {
        //TODO
    }
}