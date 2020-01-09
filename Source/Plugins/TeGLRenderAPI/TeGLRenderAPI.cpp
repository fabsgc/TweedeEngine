#include "TeGLRenderAPI.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include "Win32/TeWin32RenderWindow.h"
#elif TE_PLATFORM == TE_PLATFORM_LINUX
#   include "Linux/TeLinuxRenderWindow.h"
#endif

namespace te
{
    GLRenderAPI::GLRenderAPI()
    {
    }

    GLRenderAPI::~GLRenderAPI()
    {
    }

    SPtr<RenderWindow> GLRenderAPI::CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc)
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        return te_shared_ptr_new<Win32RenderWindow>(windowDesc);
#elif TE_PLATFORM == TE_PLATFORM_LINUX
        return te_shared_ptr_new<LinuxRenderWindow>(windowDesc);
#endif
    }

    void GLRenderAPI::Initialize()
    {
    }

    void GLRenderAPI::SetViewport(const Rect2& area)
    {
        // TODO
    }

    void GLRenderAPI::SetVertexBuffers(UINT32 index, SPtr<VertexBuffer>* buffers, UINT32 numBuffers)
    {
        // TODO
    }

    void GLRenderAPI::SetIndexBuffer(const SPtr<IndexBuffer>& buffer)
    {
        // TODO
    }

    void GLRenderAPI::Draw(UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount)
    {
        // TODO
    }

    void GLRenderAPI::DrawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount)
    {
        // TODO
    }

    void GLRenderAPI::SwapBuffers(const SPtr<RenderTarget>& target)
    {
        // TODO
    }

    void GLRenderAPI::SetRenderTarget(const SPtr<RenderTarget>& target)
    {
        // TODO
    }

    void GLRenderAPI::ClearRenderTarget(UINT32 buffers, float depth, UINT16 stencil, UINT8 targetMask)
    {
        // TODO
    }

    void GLRenderAPI::ClearViewport(UINT32 buffers, float depth, UINT16 stencil, UINT8 targetMask)
    {
        // TODO
    }
}
