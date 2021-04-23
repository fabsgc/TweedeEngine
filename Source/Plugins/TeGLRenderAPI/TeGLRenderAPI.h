#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderAPI.h"
#include "TeGLGLSLProgramFactory.h"
#include "Math/TeRect2.h"

namespace te
{
    class GLRenderAPI: public RenderAPI
    {
    public:
        GLRenderAPI();
        ~GLRenderAPI() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(GLRenderAPI)

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

        /**	Creates render system capabilities that specify which features are or aren't supported. */
        void InitCapabilities(RenderAPICapabilities& caps) const;

        /**	Finish initialization by setting up any systems dependant on render systemcapabilities. */
        void InitFromCaps(RenderAPICapabilities* caps);

        /** @copydoc RenderAPI::ConvertProjectionMatrix() */
        void ConvertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) override;

        /** @copydoc RenderAPI::GenerateParamBlockDesc() */
        GpuParamBlockDesc GenerateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params) override;

        /**
         * Switch the currently used OpenGL context. You will need to re-bind any previously bound values manually
         * (for example textures, gpu programs and such).
         */
        void SwitchContext(const SPtr<GLContext>& context, const RenderWindow& window);

        /** @copydoc RenderAPI::GetGPUMemory() */
        UINT64 GetGPUMemory() override;

        /** @copydoc RenderAPI::GetUsedGPUMemory() */
        UINT64 GetUsedGPUMemory() override;

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
        /** Information about a currently bound texture. */
        struct TextureInfo
        {
            GLenum type = GL_TEXTURE_2D;
        };

        static const UINT32 MAX_VB_COUNT = 32;

        Rect2 _viewportNorm = Rect2(0.0f, 0.0f, 1.0f, 1.0f);
        UINT32 _scissorTop = 0;
        UINT32 _scissorBottom = 720;
        UINT32 _scissorLeft = 0;
        UINT32 _scissorRight = 1280;
        UINT32 _viewportLeft = 0;
        UINT32 _viewportTop = 0;
        UINT32 _viewportWidth = 0;
        UINT32 _viewportHeight = 0;
        bool _scissorEnabled = false;
        bool _scissorRectDirty = false;

        UINT32 _stencilReadMask = 0xFFFFFFFF;
        UINT32 _stencilWriteMask = 0xFFFFFFFF;
        UINT32 _stencilRefValue = 0;
        CompareFunction _stencilCompareFront = CMPF_ALWAYS_PASS;
        CompareFunction _stencilCompareBack = CMPF_ALWAYS_PASS;

        // Last min & mip filtering options, so we can combine them
        FilterOptions _minFilter;
        FilterOptions _mipFilter;

        // Holds texture type settings for every stage
        UINT32 _numTextureUnits = 0;
        TextureInfo* _textureInfos = nullptr;
        bool _depthWrite = true;
        bool _colorWrite[TE_MAX_MULTIPLE_RENDER_TARGETS][4];

        GLSupport* _GLSupport;
        bool _GLInitialised;

        GLGLSLProgramFactory* _GLSLFactory = nullptr;

        SPtr<GLGpuProgram> _currentVertexProgram;
        SPtr<GLGpuProgram> _currentFragmentProgram;
        SPtr<GLGpuProgram> _currentGeometryProgram;
        SPtr<GLGpuProgram> _currentHullProgram;
        SPtr<GLGpuProgram> _currentDomainProgram;
        SPtr<GLGpuProgram> _currentComputeProgram;

        std::array<SPtr<VertexBuffer>, MAX_VB_COUNT> _boundVertexBuffers;
        SPtr<VertexDeclaration> _boundVertexDeclaration;
        SPtr<IndexBuffer> _boundIndexBuffer;
        DrawOperationType _currentDrawOperation = DOT_TRIANGLE_LIST;

        SPtr<GLContext> _mainContext;
        SPtr<GLContext> _currentContext;
    };
}