#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11DriverList.h"
#include "TeD3D11Driver.h"
#include "TeD3D11InputLayoutManager.h"
#include "TeD3D11HLSLProgramFactory.h"
#include "Math/TeRect2.h"
#include "TeD3D11DepthStencilState.h"
#include "TeD3D11GpuProgram.h"

#define D3D11_MAX_BOUND_VERTEX_BUFFER 32

namespace te
{
    class D3D11RenderAPI: public RenderAPI
    {
        struct LastFrameGraphicPipeline
        {
            D3D11RasterizerState* d3d11RasterizerState = nullptr;
            D3D11BlendState* d3d11BlendState = nullptr;
            D3D11DepthStencilState* d3d11DepthStencilState = nullptr;

            D3D11GpuProgram* d3d11VertexProgram = nullptr;
            D3D11GpuPixelProgram* d3d11PixelProgram = nullptr;
            D3D11GpuGeometryProgram* d3d11GeometryProgram = nullptr;
            D3D11GpuDomainProgram* d3d11DomainProgram = nullptr;
            D3D11GpuHullProgram* d3d11HullProgram = nullptr;
            D3D11GpuComputeProgram* d3d11ComputeProgram = nullptr;

            DrawOperationType drawOperationType = DOT_TRIANGLE_LIST;

            ID3D11InputLayout* ia = nullptr;

            ID3D11Buffer* vertexBuffer = nullptr;
        };

        struct GpuResourcesContainer {
            GpuResourcesContainer()
            {
                srvs.reserve(8);
                uavs.reserve(8);
                constBuffers.reserve(8);
                samplers.reserve(8);
            }

            Vector<ID3D11ShaderResourceView*> srvs;
            Vector<ID3D11UnorderedAccessView*> uavs;
            Vector<ID3D11Buffer*> constBuffers;
            Vector<ID3D11SamplerState*> samplers;
        };

    public:
        D3D11RenderAPI();
        ~D3D11RenderAPI();

        TE_MODULE_STATIC_HEADER_MEMBER(D3D11RenderAPI)

        SPtr<RenderWindow> CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc) override;
        void Initialize() override;

        /** Returns the main DXGI factory object. */
        IDXGIFactory1* GetDXGIFactory() const { return _DXGIFactory; }

        /** Returns the primary DX11 device object. */
        D3D11Device& GetPrimaryDevice() const { return *_device; }

        /** Returns information describing all available drivers. */
        D3D11DriverList* GetDriverList() const { return _driverList; }

        void Destroy() override;

        /** @copydoc RenderAPI::SetGraphicsPipeline */
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

        /** @copydoc RenderAPI::DispatchCompute */
        void DispatchCompute(UINT32 numGroupsX, UINT32 numGroupsY = 1, UINT32 numGroupsZ = 1) override;

        /** @copydoc RenderAPI::SwapBuffers */
        void SwapBuffers(const SPtr<RenderTarget>& target) override;

        /** @copydoc RenderAPI::SetRenderTarget */
        void SetRenderTarget(const SPtr<RenderTarget>& target, UINT32 readOnlyFlags) override;

        /** @copydoc RenderAPI::ClearRenderTarget */
        void ClearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF) override;

        /** @copydoc RenderAPI::ClearViewport */
        void ClearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF) override;

        /**
         * Determines DXGI multisample settings from the provided parameters.
         *
         * @param[in]	multisampleCount	Number of requested samples.
         * @param[in]	format				Pixel format used by the render target.
         * @param[out]	outputSampleDesc	Output structure that will contain the requested multisample settings.
         */
        void DetermineMultisampleSettings(UINT32 multisampleCount, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outputSampleDesc);

    private:
        /**
         * Creates or retrieves a proper input layout depending on the currently set vertex shader and vertex buffer.
         * Applies the input layout to the pipeline.
         */
        void ApplyInputLayout();

        /**
         * Recalculates actual viewport dimensions based on currently set viewport normalized dimensions and render target
         * and applies them for further rendering.
         */
        void ApplyViewport();

        /** Notifies the active render target that a rendering command was queued that will potentially change its contents. */
        void NotifyRenderTargetModified();

        /** Creates and populates a set of render system capabilities describing which functionality is available. */
        void InitCapabilites(IDXGIAdapter* adapter, RenderAPICapabilities& caps) const;

        /** @copydoc RenderAPI::ConvertProjectionMatrix */
        void ConvertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) override;

        /** @copydoc RenderAPI::GenerateParamBlockDesc */
        GpuParamBlockDesc GenerateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params) override;

        /** @copydoc RenderAPI::GetGPUMemory */
        UINT64 GetGPUMemory() override;

        /** @copydoc RenderAPI::GetSharedMemory */
        UINT64 GetSharedMemory() override;

        /** @copydoc RenderAPI::GetUsedGPUMemory */
        UINT64 GetUsedGPUMemory() override;

    private:
        static bool MemoryQuerySupport;

    private:
        SPtr<LastFrameGraphicPipeline> _lastFrameGraphicPipeline;
        GpuResourcesContainer _gpuResContainer;

        ID3D11RenderTargetView** _activeViews = nullptr;

        IDXGIFactory1* _DXGIFactory = nullptr;
        IDXGIAdapter* _selectedAdapter = nullptr;
        D3D11Device* _device = nullptr;

        D3D11DriverList* _driverList = nullptr;
        D3D11Driver* _activeD3DDriver = nullptr;

        D3D_FEATURE_LEVEL _featureLevel = ::D3D_FEATURE_LEVEL_11_0;

        D3D11HLSLProgramFactory* _HLSLFactory = nullptr;
        D3D11InputLayoutManager* _IAManager = nullptr;

        bool _PSUAVsBound = false;
        bool _CSUAVsBound = false;

        UINT32 _stencilRef = 0;
        Rect2 _viewportNorm = Rect2(0.0f, 0.0f, 1.0f, 1.0f);
        D3D11_VIEWPORT _viewport;
        D3D11_RECT _scissorRect;

        SPtr<VertexDeclaration> _activeVertexDeclaration;
        SPtr<D3D11GpuProgram>  _activeVertexShader;
        SPtr<D3D11DepthStencilState> _activeDepthStencilState;

        DrawOperationType _activeDrawOp = DOT_TRIANGLE_LIST;
    };
}
