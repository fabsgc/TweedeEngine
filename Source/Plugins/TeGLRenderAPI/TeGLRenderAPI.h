#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderAPI.h"
#include "TeGLGLSLProgramFactory.h"

namespace te
{
    class GLRenderAPI: public RenderAPI
    {
    public:
        GLRenderAPI();
        ~GLRenderAPI();

        SPtr<RenderWindow> CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc) override;
        void Initialize() override;
        void Destroy() override;

        /** @copydoc RenderAPI::setGraphicsPipeline */
        void SetGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState) override;

        /** @copydoc RenderAPI::SetComputePipeline */
        void SetComputePipeline(const SPtr<ComputePipelineState>& pipelineState) override;

        /** @copydoc RenderAPI::SetGpuParams */
        void SetGpuParams(const SPtr<GpuParams>& gpuParams, UINT32 gpuParamsBindFlags = (UINT32)GPU_BIND_ALL,
            UINT32 gpuParamsBlockBindFlags = (UINT32)GPU_BIND_PARAM_BLOCK_ALL, const Vector<String>& paramBlocksToBind = {}) override;
        
        /** @copydoc RenderAPI::SetViewport */
        void SetViewport(const Rect2& area) override;

        /** @copydoc RenderAPI::SetScissorRect */
        void SetScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom) override;

        /** @copydoc RenderAPI::SetStencilRef */
        void SetStencilRef(UINT32 value) override;

        /** @copydoc RenderAPI::SetVertexBuffers */
        void SetVertexBuffers(UINT32 index, SPtr<VertexBuffer>* buffers, UINT32 numBuffers) override;

        /** @copydoc RenderAPI::SetIndexBuffer */
        void SetIndexBuffer(const SPtr<IndexBuffer>& buffer) override;

        /** @copydoc RenderAPI::SetVertexDeclaration */
        void SetVertexDeclaration(const SPtr<VertexDeclaration>& vertexDeclaration) override;

        /** @copydoc RenderAPI::SetDrawOperation */
        void SetDrawOperation(DrawOperationType op) override;

        /** @copydoc RenderAPI::Draw */
        void Draw(UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount = 0) override;

        /** @copydoc RenderAPI::DrawIndexed */
        void DrawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount = 0) override;

        /** @copydoc RenderAPI::DispatchCompute() */
        void DispatchCompute(UINT32 numGroupsX, UINT32 numGroupsY = 1, UINT32 numGroupsZ = 1) override;

        /** @copydoc RenderAPI::SwapBuffers */
        void SwapBuffers(const SPtr<RenderTarget>& target) override;

        /** @copydoc RenderAPI::SetRenderTarget */
        void SetRenderTarget(const SPtr<RenderTarget>& target, UINT32 readOnlyFlags) override;

        /** @copydoc RenderAPI::ClearRenderTarget */
        void ClearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF) override;

        /** @copydoc RenderAPI::ClearViewport */
        void ClearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF) override;

        /** @copydoc RenderAPI::ConvertProjectionMatrix() */
        void ConvertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) override;

        /** @copydoc RenderAPI::GenerateParamBlockDesc() */
        GpuParamBlockDesc GenerateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params) override;

        /**	Creates render system capabilities that specify which features are or aren't supported. */
        void InitCapabilities(RenderAPICapabilities& caps) const;

        /**	Finish initialization by setting up any systems dependant on render systemcapabilities. */
        void InitFromCaps(RenderAPICapabilities* caps);

        /************************************************************************/
        /* 				Internal use by OpenGL RenderSystem only                */
        /************************************************************************/

        /**	Query has the main context been initialized. */
        bool IsContextInitialized() const { return _GLInitialised; }

        /**	Returns main context. Caller must ensure the context has been initialized. */
        SPtr<GLContext> GetMainContext() const { return _mainContext; }

        /**	Returns a support object you may use for creating */
        GLSupport* GetGLSupport() const { return _GLSupport; }

    private:
        GLGLSLProgramFactory* _GLSLFactory = nullptr;

        GLSupport* _GLSupport;
        bool _GLInitialised;

        SPtr<GLContext> _mainContext;
        SPtr<GLContext> _currentContext;
    };
}