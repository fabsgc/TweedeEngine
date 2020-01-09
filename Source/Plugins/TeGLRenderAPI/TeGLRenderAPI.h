#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderAPI.h"

namespace te
{
    class GLRenderAPI: public RenderAPI
    {
    public:
        GLRenderAPI();
        ~GLRenderAPI();

        SPtr<RenderWindow> CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc) override;
        void Initialize() override;
        
        /** @copydoc RenderAPI::SetViewport */
        void SetViewport(const Rect2& area) override;

        /** @copydoc RenderAPI::SetVertexBuffers */
        void SetVertexBuffers(UINT32 index, SPtr<VertexBuffer>* buffers, UINT32 numBuffers) override;

        /** @copydoc RenderAPI::SetIndexBuffer */
        void SetIndexBuffer(const SPtr<IndexBuffer>& buffer) override;

        /** @copydoc RenderAPI::Draw */
        void Draw(UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount = 0) override;

        /** @copydoc RenderAPI::DrawIndexed */
        void DrawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount = 0) override;

        /** @copydoc RenderAPI::SwapBuffers */
        void SwapBuffers(const SPtr<RenderTarget>& target) override;

        /** @copydoc RenderAPI::SetRenderTarget */
        void SetRenderTarget(const SPtr<RenderTarget>& target) override;

        /** @copydoc RenderAPI::ClearRenderTarget */
        void ClearRenderTarget(UINT32 buffers, float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF) override;

        /** @copydoc RenderAPI::ClearViewport */
        void ClearViewport(UINT32 buffers, float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF) override;
    };
}