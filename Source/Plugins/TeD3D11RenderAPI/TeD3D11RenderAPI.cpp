#include "TeD3D11RenderAPI.h"
#include "TeD3D11RenderWindow.h"
#include "Math/TeRect2I.h"
#include "TeD3D11Mappings.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "TeD3D11RasterizerState.h"
#include "TeD3D11BlendState.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "TeD3D11RenderStateManager.h"
#include "TeD3D11TextureManager.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "TeD3D11GpuProgram.h"
#include "TeD3D11HardwareBufferManager.h"
#include "TeD3D11VertexBuffer.h"
#include "TeD3D11IndexBuffer.h"

#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeGpuParamDesc.h"

#include "TeD3D11TextureView.h"
#include "TeD3D11SamplerState.h"

namespace te
{
    D3D11RenderAPI::D3D11RenderAPI()
        : _viewport()
        , _scissorRect(D3D11_RECT())
    { }

    D3D11RenderAPI::~D3D11RenderAPI()
    { }

    SPtr<RenderWindow> D3D11RenderAPI::CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc)
    {
        SPtr<D3D11RenderWindow> window = te_shared_ptr_new<D3D11RenderWindow>(windowDesc, *_device, _DXGIFactory);
        window->SetThisPtr(window);

        return window;
    }

    void D3D11RenderAPI::Initialize()
    {
        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&_DXGIFactory);
        if (FAILED(hr))
        {
            TE_ASSERT_ERROR(false, "Failed to create Direct3D11 DXGIFactory", __FILE__, __LINE__);
        }

        _driverList = te_new<D3D11DriverList>(_DXGIFactory);
        _activeD3DDriver = _driverList->Item(0);
		_videoModeInfo = _activeD3DDriver->GetVideoModeInfo();

        IDXGIAdapter* selectedAdapter = _activeD3DDriver->GetDeviceAdapter();

        D3D_FEATURE_LEVEL requestedLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        const UINT32 numRequestedLevels = sizeof(requestedLevels) / sizeof(requestedLevels[0]);

        UINT32 deviceFlags = 0;
#if TE_DEBUG_MODE
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        ID3D11Device* device;
        hr = D3D11CreateDevice(selectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, deviceFlags,
            requestedLevels, numRequestedLevels, D3D11_SDK_VERSION, &device, &_featureLevel, nullptr);

        // This will fail on Win 7 due to lack of 11.1, so re-try again without it
        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(selectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, deviceFlags,
                &requestedLevels[1], numRequestedLevels - 1, D3D11_SDK_VERSION, &device, &_featureLevel, nullptr);
        }

        if (FAILED(hr))
        {
            TE_ASSERT_ERROR(false, "Failed to create Direct3D11 object. D3D11CreateDeviceN returned this error code: " + ToString(hr), __FILE__, __LINE__);
        }

        _device = te_new<D3D11Device>(device);

        // Create the texture manager for use by others		
        TextureManager::StartUp<D3D11TextureManager>();

        // Create hardware buffer manager
        HardwareBufferManager::StartUp<D3D11HardwareBufferManager>(std::ref(*_device));

        // Create & register HLSL factory		
        _HLSLFactory = te_new<D3D11HLSLProgramFactory>();
        GpuProgramManager::Instance().AddFactory("hlsl", _HLSLFactory);

        // Create render state manager
        RenderStateManager::StartUp<D3D11RenderStateManager>();

        // Create Input Layout Manager	
        _IAManager = te_new<D3D11InputLayoutManager>();

        RenderAPI::Initialize();
    }

    void D3D11RenderAPI::Destroy()
    {
        // Ensure that all GPU commands finish executing before shutting down the device. If we don't do this a crash
        // on shutdown may occurr as the driver is still executing the commands, and we unload this library.
        _device->GetImmediateContext()->Flush();

        if (_IAManager != nullptr)
        {
            te_delete(_IAManager);
            _IAManager = nullptr;
        }

        if (_HLSLFactory != nullptr)
        {
            te_delete(_HLSLFactory);
            _HLSLFactory = nullptr;
        }

        SAFE_RELEASE(_DXGIFactory);

        if (_device != nullptr)
        {
            te_delete(_device);
            _device = nullptr;
        }

        if (_driverList != nullptr)
        {
            te_delete(_driverList);
            _driverList = nullptr;
        }

        _activeVertexDeclaration = nullptr;
		_activeVertexShader = nullptr;
        _activeRenderTarget = nullptr;
        _activeD3DDriver = nullptr;
        _activeDepthStencilState = nullptr;
        _activeVertexDeclaration = nullptr;

        TextureManager::ShutDown();
        RenderStateManager::ShutDown();
        HardwareBufferManager::ShutDown();

        SAFE_RELEASE(_DXGIFactory);

        RenderAPI::Destroy();
    }

    void D3D11RenderAPI::SetGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState)
    {
        D3D11RasterizerState* d3d11RasterizerState;
        D3D11BlendState* d3d11BlendState;

        D3D11GpuPixelProgram* d3d11PixelProgram;
        D3D11GpuGeometryProgram* d3d11GeometryProgram;
        D3D11GpuDomainProgram* d3d11DomainProgram;
        D3D11GpuHullProgram* d3d11HullProgram;

        if (pipelineState != nullptr)
        {
            d3d11BlendState = static_cast<D3D11BlendState*>(pipelineState->GetBlendState().get());
            d3d11RasterizerState = static_cast<D3D11RasterizerState*>(pipelineState->GetRasterizerState().get());
            _activeDepthStencilState = std::static_pointer_cast<D3D11DepthStencilState>(pipelineState->GetDepthStencilState());

            _activeVertexShader = std::static_pointer_cast<D3D11GpuVertexProgram>(pipelineState->GetVertexProgram());
            d3d11PixelProgram = static_cast<D3D11GpuPixelProgram*>(pipelineState->GetPixelProgram().get());
            d3d11GeometryProgram = static_cast<D3D11GpuGeometryProgram*>(pipelineState->GetGeometryProgram().get());
            d3d11DomainProgram = static_cast<D3D11GpuDomainProgram*>(pipelineState->GetDomainProgram().get());
            d3d11HullProgram = static_cast<D3D11GpuHullProgram*>(pipelineState->GetHullProgram().get());

            if (d3d11BlendState == nullptr)
            {
                d3d11BlendState = static_cast<D3D11BlendState*>(BlendState::GetDefault().get());
            }

            if (d3d11RasterizerState == nullptr)
            {
                d3d11RasterizerState = static_cast<D3D11RasterizerState*>(RasterizerState::GetDefault().get());
            }

            if (_activeDepthStencilState == nullptr)
            {
                _activeDepthStencilState = std::static_pointer_cast<D3D11DepthStencilState>(DepthStencilState::GetDefault());
            }
        }
        else
        {
            d3d11BlendState = static_cast<D3D11BlendState*>(BlendState::GetDefault().get());
            d3d11RasterizerState = static_cast<D3D11RasterizerState*>(RasterizerState::GetDefault().get());
            _activeDepthStencilState = std::static_pointer_cast<D3D11DepthStencilState>(DepthStencilState::GetDefault());

            _activeVertexShader = nullptr;
            d3d11PixelProgram = nullptr;
            d3d11GeometryProgram = nullptr;
            d3d11DomainProgram = nullptr;
            d3d11HullProgram = nullptr;
        }

        ID3D11DeviceContext* d3d11Context = _device->GetImmediateContext();
        d3d11Context->OMSetBlendState(d3d11BlendState->GetInternal(), nullptr, 0xFFFFFFFF);
        d3d11Context->RSSetState(d3d11RasterizerState->GetInternal());
        d3d11Context->OMSetDepthStencilState(_activeDepthStencilState->GetInternal(), _stencilRef);

        if (_activeVertexShader != nullptr)
        {
            D3D11GpuVertexProgram* vertexProgram = static_cast<D3D11GpuVertexProgram*>(_activeVertexShader.get());
            d3d11Context->VSSetShader(vertexProgram->GetVertexShader(), nullptr, 0);
        }
        else
        {
            d3d11Context->VSSetShader(nullptr, nullptr, 0);
        }

        if (d3d11PixelProgram != nullptr)
        {
            d3d11Context->PSSetShader(d3d11PixelProgram->GetPixelShader(), nullptr, 0);
        }
        else
        {
            d3d11Context->PSSetShader(nullptr, nullptr, 0);
        }

        if (d3d11GeometryProgram != nullptr)
        {
            d3d11Context->GSSetShader(d3d11GeometryProgram->GetGeometryShader(), nullptr, 0);
        }
        else
        {
            d3d11Context->GSSetShader(nullptr, nullptr, 0);
        }

        if (d3d11DomainProgram != nullptr)
        {
            d3d11Context->DSSetShader(d3d11DomainProgram->GetDomainShader(), nullptr, 0);
        }
        else
        {
            d3d11Context->DSSetShader(nullptr, nullptr, 0);
        }

        if (d3d11HullProgram != nullptr)
        {
            d3d11Context->HSSetShader(d3d11HullProgram->GetHullShader(), nullptr, 0);
        }
        else 
        {
            d3d11Context->HSSetShader(nullptr, nullptr, 0);
        }
    }

    void D3D11RenderAPI::SetGpuParams(const SPtr<GpuParams>& gpuParams)
    {
        ID3D11DeviceContext* context = _device->GetImmediateContext();

        // Clear any previously bound UAVs (otherwise shaders attempting to read resources viewed by those views will be unable to)
        if (_PSUAVsBound || _CSUAVsBound)
        {
            ID3D11UnorderedAccessView* emptyUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT];
            te_zero_out(emptyUAVs);

            if (_PSUAVsBound)
            {
                context->OMSetRenderTargetsAndUnorderedAccessViews(
                    D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0,
                    D3D11_PS_CS_UAV_REGISTER_COUNT, emptyUAVs, nullptr);

                _PSUAVsBound = false;
            }

            if (_CSUAVsBound)
            {
                context->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, emptyUAVs, nullptr);
                _CSUAVsBound = false;
            }
        }

        Vector<ID3D11ShaderResourceView*> srvs(8);
        Vector<ID3D11UnorderedAccessView*> uavs(8);
        Vector<ID3D11Buffer*> constBuffers(8);
        Vector<ID3D11SamplerState*> samplers(8);

        auto populateViews = [&](GpuProgramType type)
        {
            srvs.clear();
            uavs.clear();
            constBuffers.clear();
            samplers.clear();

            SPtr<GpuParamDesc> paramDesc = gpuParams->GetParamDesc(type);
            if (paramDesc == nullptr)
            {
                return;
            }

            for (auto iter = paramDesc->Textures.begin(); iter != paramDesc->Textures.end(); ++iter)
            {
                // TODO
            }

            for (auto iter = paramDesc->Buffers.begin(); iter != paramDesc->Buffers.end(); ++iter)
            {
                // TODO
            }

            for (auto iter = paramDesc->Samplers.begin(); iter != paramDesc->Samplers.end(); ++iter)
            {
                // TODO
            }

            for (auto iter = paramDesc->ParamBlocks.begin(); iter != paramDesc->ParamBlocks.end(); ++iter)
            {
                // TODO
            }
        };

        UINT32 numSRVs = 0;
        UINT32 numUAVs = 0;
        UINT32 numConstBuffers = 0;
        UINT32 numSamplers = 0;

        populateViews(GPT_VERTEX_PROGRAM);
        numSRVs = (UINT32)srvs.size();
        numConstBuffers = (UINT32)constBuffers.size();
        numSamplers = (UINT32)samplers.size();

        if (numSRVs > 0) context->VSSetShaderResources(0, numSRVs, srvs.data());
        if (numConstBuffers > 0) context->VSSetConstantBuffers(0, numConstBuffers, constBuffers.data());
        if (numSamplers > 0) context->VSSetSamplers(0, numSamplers, samplers.data());

        populateViews(GPT_PIXEL_PROGRAM);
        numSRVs = (UINT32)srvs.size();
        numUAVs = (UINT32)uavs.size();
        numConstBuffers = (UINT32)constBuffers.size();
        numSamplers = (UINT32)samplers.size();

        if (numSRVs > 0) context->PSSetShaderResources(0, numSRVs, srvs.data());

        if (numUAVs > 0)
        {
            context->OMSetRenderTargetsAndUnorderedAccessViews(
                D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0, numUAVs, uavs.data(), nullptr);
            _PSUAVsBound = true;
        }

        if (numConstBuffers > 0) context->PSSetConstantBuffers(0, numConstBuffers, constBuffers.data());
        if (numSamplers > 0) context->PSSetSamplers(0, numSamplers, samplers.data());

        populateViews(GPT_GEOMETRY_PROGRAM);
        numSRVs = (UINT32)srvs.size();
        numConstBuffers = (UINT32)constBuffers.size();
        numSamplers = (UINT32)samplers.size();

        if (numSRVs > 0) context->GSSetShaderResources(0, numSRVs, srvs.data());
        if (numConstBuffers > 0) context->GSSetConstantBuffers(0, numConstBuffers, constBuffers.data());
        if (numSamplers > 0) context->GSSetSamplers(0, numSamplers, samplers.data());

        populateViews(GPT_HULL_PROGRAM);
        numSRVs = (UINT32)srvs.size();
        numConstBuffers = (UINT32)constBuffers.size();
        numSamplers = (UINT32)samplers.size();

        if (numSRVs > 0) context->HSSetShaderResources(0, numSRVs, srvs.data());
        if (numConstBuffers > 0) context->HSSetConstantBuffers(0, numConstBuffers, constBuffers.data());
        if (numSamplers > 0) context->HSSetSamplers(0, numSamplers, samplers.data());

        populateViews(GPT_DOMAIN_PROGRAM);
        numSRVs = (UINT32)srvs.size();
        numConstBuffers = (UINT32)constBuffers.size();
        numSamplers = (UINT32)samplers.size();

        if (numSRVs > 0) context->DSSetShaderResources(0, numSRVs, srvs.data());
        if (numConstBuffers > 0) context->DSSetConstantBuffers(0, numConstBuffers, constBuffers.data());
        if (numSamplers > 0) context->DSSetSamplers(0, numSamplers, samplers.data());
    }

    void D3D11RenderAPI::SetViewport(const Rect2& area)
    {
        _viewportNorm = area;
        ApplyViewport();
    }

    void D3D11RenderAPI::SetScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
    {
        _scissorRect.left = static_cast<LONG>(left);
        _scissorRect.top = static_cast<LONG>(top);
        _scissorRect.bottom = static_cast<LONG>(bottom);
        _scissorRect.right = static_cast<LONG>(right);

        _device->GetImmediateContext()->RSSetScissorRects(1, &_scissorRect);
    }

    void D3D11RenderAPI::SetStencilRef(UINT32 value)
    {
        _stencilRef = value;

        if(_activeDepthStencilState != nullptr)
        {
            _device->GetImmediateContext()->OMSetDepthStencilState(_activeDepthStencilState->GetInternal(), _stencilRef);
        }
        else
        {
            _device->GetImmediateContext()->OMSetDepthStencilState(nullptr, _stencilRef);
        }
    }

    void D3D11RenderAPI::SetVertexBuffers(UINT32 index, SPtr<VertexBuffer>* buffers, UINT32 numBuffers)
    {
        UINT32 maxBoundVertexBuffers = D3D11_MAX_BOUND_VERTEX_BUFFER;
        if (index < 0 || (index + numBuffers) >= maxBoundVertexBuffers)
        {
            TE_ASSERT_ERROR(false, "Invalid vertex index: " + ToString(index) +
                ". Valid range is 0 .. " + ToString(maxBoundVertexBuffers - 1), __FILE__, __LINE__);
        }

        ID3D11Buffer* dx11buffers[D3D11_MAX_BOUND_VERTEX_BUFFER];
        UINT32 strides[D3D11_MAX_BOUND_VERTEX_BUFFER];
        UINT32 offsets[D3D11_MAX_BOUND_VERTEX_BUFFER];

        for (UINT32 i = 0; i < numBuffers; i++)
        {
            SPtr<D3D11VertexBuffer> vertexBuffer = std::static_pointer_cast<D3D11VertexBuffer>(buffers[i]);
            const VertexBufferProperties& vbProps = vertexBuffer->GetProperties();

            dx11buffers[i] = vertexBuffer->GetD3DVertexBuffer();

            strides[i] = vbProps.GetVertexSize();
            offsets[i] = 0;
        }

        _device->GetImmediateContext()->IASetVertexBuffers(index, numBuffers, dx11buffers, strides, offsets);
    }

    void D3D11RenderAPI::SetIndexBuffer(const SPtr<IndexBuffer>& buffer)
    {
        SPtr<D3D11IndexBuffer> indexBuffer = std::static_pointer_cast<D3D11IndexBuffer>(buffer);

        DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
        if (indexBuffer->GetProperties().GetType() == IT_16BIT)
        {
            indexFormat = DXGI_FORMAT_R16_UINT;
        }
        else if (indexBuffer->GetProperties().GetType() == IT_32BIT)
        {
            indexFormat = DXGI_FORMAT_R32_UINT;
        }
        else
        {
            TE_ASSERT_ERROR(false, "Unsupported index format: " + ToString(indexBuffer->GetProperties().GetType()), __FILE__, __LINE__);
        }

        _device->GetImmediateContext()->IASetIndexBuffer(indexBuffer->GetD3DIndexBuffer(), indexFormat, 0);
    }

    void D3D11RenderAPI::SetVertexDeclaration(const SPtr<VertexDeclaration>& vertexDeclaration)
    {
        _activeVertexDeclaration = vertexDeclaration;
    }

    void D3D11RenderAPI::SetDrawOperation(DrawOperationType op)
    {
        _device->GetImmediateContext()->IASetPrimitiveTopology(D3D11Mappings::GetPrimitiveType(op));
		_activeDrawOp = op;
    }

    void D3D11RenderAPI::Draw(UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount)
    {
        ApplyInputLayout();

        if (instanceCount <= 1)
        {
            _device->GetImmediateContext()->Draw(vertexCount, vertexOffset);
        }
        else
        {
            _device->GetImmediateContext()->DrawInstanced(vertexCount, instanceCount, vertexOffset, 0);
        }

#if TE_DEBUG_MODE
		if (_device->HasError())
        {
			TE_DEBUG(_device->GetErrorDescription(), __FILE__, __LINE__);
        }
#endif

        NotifyRenderTargetModified();
    }

    void D3D11RenderAPI::DrawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount)
    {
        ApplyInputLayout();

        if (instanceCount <= 1)
        {
            _device->GetImmediateContext()->DrawIndexed(indexCount, startIndex, vertexOffset);
        }
        else
        {
            _device->GetImmediateContext()->DrawIndexedInstanced(indexCount, instanceCount, startIndex, vertexOffset, 0);
        }

#if TE_DEBUG_MODE
			if (_device->HasError())
            {
				TE_DEBUG(_device->GetErrorDescription(), __FILE__, __LINE__);
            }
#endif

        NotifyRenderTargetModified();
    }

    void D3D11RenderAPI::SwapBuffers(const SPtr<RenderTarget>& target)
    {
        target->SwapBuffers();
    }

    void D3D11RenderAPI::SetRenderTarget(const SPtr<RenderTarget>& target)
    {
        _activeRenderTarget = target;
        _activeRenderTargetModified = false;

        UINT32 maxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
        ID3D11RenderTargetView** views = te_newN<ID3D11RenderTargetView*>(maxRenderTargets);
        memset(views, 0, sizeof(ID3D11RenderTargetView*) * maxRenderTargets);

        ID3D11DepthStencilView* depthStencilView = nullptr;

        if (target != nullptr)
        {
            target->GetCustomAttribute("RTV", views);
            target->GetCustomAttribute("RODSV", &depthStencilView);
        }

        // Bind render targets
        _device->GetImmediateContext()->OMSetRenderTargets(maxRenderTargets, views, depthStencilView);

        if (_device->HasError())
        {
            TE_ASSERT_ERROR(false, "Failed to setRenderTarget : " + _device->GetErrorDescription(), __FILE__, __LINE__);
        }

        te_deleteN(views, maxRenderTargets);
        ApplyViewport();
    }

    void D3D11RenderAPI::ClearRenderTarget(UINT32 buffers, const Color& color, float depth, UINT16 stencil, UINT8 targetMask)
    {
        if (_activeRenderTarget == nullptr)
        {
            return;
        }

        // Clear render surfaces
        if (buffers & FBT_COLOR)
        {
            UINT32 maxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

            ID3D11RenderTargetView** views = te_newN<ID3D11RenderTargetView*>(maxRenderTargets);
            memset(views, 0, sizeof(ID3D11RenderTargetView*) * maxRenderTargets);

            _activeRenderTarget->GetCustomAttribute("RTV", views);
            if (!views[0])
            {
                te_deleteN(views, maxRenderTargets);
                return;
            }

            float clearColor[4];
            clearColor[0] = color.r;
            clearColor[1] = color.g;
            clearColor[2] = color.b;
            clearColor[3] = color.a;

            for (UINT32 i = 0; i < maxRenderTargets; i++)
            {
                if (views[i] != nullptr && ((1 << i) & targetMask) != 0)
                {
                    _device->GetImmediateContext()->ClearRenderTargetView(views[i], clearColor);
                }
            }

            te_deleteN(views, maxRenderTargets);
        }

        // Clear depth stencil
        if ((buffers & FBT_DEPTH) != 0 || (buffers & FBT_STENCIL) != 0)
        {
            ID3D11DepthStencilView* depthStencilView = nullptr;
            _activeRenderTarget->GetCustomAttribute("DSV", &depthStencilView);

            if (depthStencilView != nullptr)
            {
                D3D11_CLEAR_FLAG clearFlag;

                if ((buffers & FBT_DEPTH) != 0 && (buffers & FBT_STENCIL) != 0)
                {
                    clearFlag = (D3D11_CLEAR_FLAG)(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
                }
                else if ((buffers & FBT_STENCIL) != 0)
                {
                    clearFlag = D3D11_CLEAR_STENCIL;
                }
                else
                {
                    clearFlag = D3D11_CLEAR_DEPTH;
                }

                if (depthStencilView != nullptr)
                {
                    _device->GetImmediateContext()->ClearDepthStencilView(depthStencilView, clearFlag, depth, (UINT8)stencil);
                }
            }
        }

        NotifyRenderTargetModified();
    }

    void D3D11RenderAPI::ClearViewport(UINT32 buffers, const Color& color, float depth, UINT16 stencil, UINT8 targetMask)
    {
        if (_activeRenderTarget == nullptr)
        {
            return;
        }

        const RenderTargetProperties& rtProps = _activeRenderTarget->GetProperties();

        Rect2I clearArea((int)_viewport.TopLeftX, (int)_viewport.TopLeftY, (int)_viewport.Width, (int)_viewport.Height);

        bool clearEntireTarget = clearArea.width == 0 || clearArea.height == 0;
        clearEntireTarget |= (clearArea.x == 0 && clearArea.y == 0 && clearArea.width == rtProps.Width &&
            clearArea.height == rtProps.Height);

        if (!clearEntireTarget)
        {
            // D3D11RenderUtility::Instance().DrawClearQuad(buffers, color, depth, stencil); TODO
            ClearRenderTarget(buffers, color, depth, stencil, targetMask);
            NotifyRenderTargetModified();
        }
        else
        {
            ClearRenderTarget(buffers, color, depth, stencil, targetMask);
        }
    }

    void D3D11RenderAPI::DetermineMultisampleSettings(UINT32 multisampleCount, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outputSampleDesc)
    {
        if (multisampleCount == 0 || multisampleCount == 1)
        {
            outputSampleDesc->Count = 1;
            outputSampleDesc->Quality = 0;

            return;
        }

        bool foundValid = false;
        size_t origNumSamples = multisampleCount;

        outputSampleDesc->Count = multisampleCount == 0 ? 1 : multisampleCount;
        outputSampleDesc->Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;

        while (!foundValid)
        {
            HRESULT hr = S_OK;
            UINT outQuality = 0;
            hr = _device->GetD3D11Device()->CheckMultisampleQualityLevels(format, outputSampleDesc->Count, &outQuality);

            if (SUCCEEDED(hr))
            {
                outputSampleDesc->Quality = outQuality - 1;
                foundValid = true;
            }
            else
            {
                // Drop samples
                multisampleCount--;

                if (multisampleCount == 1)
                {
                    // Ran out of options, no multisampling
                    multisampleCount = 0;
                    foundValid = true;
                }
            }
        }
    }

    void D3D11RenderAPI::ApplyInputLayout()
    {
        if (_activeVertexDeclaration == nullptr)
        {
            TE_DEBUG("Cannot apply input layout without a vertex declaration. Set vertex declaration before calling this method.", __FILE__, __LINE__);
            return;
        }

        if (_activeVertexShader == nullptr)
        {
            TE_DEBUG("Cannot apply input layout without a vertex shader. Set vertex shader before calling this method.", __FILE__, __LINE__);
            return;
        }

        ID3D11InputLayout* ia = _IAManager->RetrieveInputLayout(_activeVertexShader->GetInputDeclaration(), _activeVertexDeclaration, *_activeVertexShader);
        _device->GetImmediateContext()->IASetInputLayout(ia);
    }

    void D3D11RenderAPI::ApplyViewport()
    {
        if (_activeRenderTarget == nullptr)
        {
            return;
        }

        const RenderTargetProperties& rtProps = _activeRenderTarget->GetProperties();

        // Set viewport dimensions
        _viewport.TopLeftX = (FLOAT)(rtProps.Width * _viewportNorm.x);
        _viewport.TopLeftY = (FLOAT)(rtProps.Height * _viewportNorm.y);
        _viewport.Width = (FLOAT)(rtProps.Width * _viewportNorm.width);
        _viewport.Height = (FLOAT)(rtProps.Height * _viewportNorm.height);

        if (rtProps.RequiresTextureFlipping)
        {
            // Convert "top-left" to "bottom-left"
            _viewport.TopLeftY = rtProps.Height - _viewport.Height - _viewport.TopLeftY;
        }

        _viewport.MinDepth = 0.0f;
        _viewport.MaxDepth = 1.0f;

        _device->GetImmediateContext()->RSSetViewports(1, &_viewport);

        if (_device->HasError())
        {
            TE_ASSERT_ERROR(false, "Unable to set Viewport" + _device->GetErrorDescription(), __FILE__, __LINE__);
        }
    }

    void D3D11RenderAPI::NotifyRenderTargetModified()
    {
        if (_activeRenderTarget == nullptr || _activeRenderTargetModified)
        {
            return;
        }

        _activeRenderTargetModified = true;
    }
}
