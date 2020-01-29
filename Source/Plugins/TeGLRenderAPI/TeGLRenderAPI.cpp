#include "TeGLRenderAPI.h"
#include "Image/TeTextureManager.h"
#include "TeGLTextureManager.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "TeGLRenderStateManager.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "TeGLHardwareBufferManager.h"
#include "TeGLGLSLParamParser.h"

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
        return te_core_ptr_new<Win32RenderWindow>(windowDesc);
#elif TE_PLATFORM == TE_PLATFORM_LINUX
        return te_core_ptr_new<LinuxRenderWindow>(windowDesc);
#endif
    }

    void GLRenderAPI::Initialize()
    {
        // Create the texture manager for use by others
        TextureManager::StartUp<GLTextureManager>();

        // Create render state manager
        RenderStateManager::StartUp<GLRenderStateManager>();

        // Create hardware buffer manager
        HardwareBufferManager::StartUp<GLHardwareBufferManager>();

        // Create & register GLSL factory
        _GLSLFactory = te_new<GLGLSLProgramFactory>();
        GpuProgramManager::Instance().AddFactory("glsl", _GLSLFactory);

        _numDevices = 1;
        _capabilities = te_newN<RenderAPICapabilities>(_numDevices);
        InitCapabilities(_capabilities[0]);
    }

    void GLRenderAPI::InitCapabilities(RenderAPICapabilities& caps) const
    {
        caps.Convention.UV_YAxis = Conventions::Axis::Up;
        caps.Convention.matrixOrder = Conventions::MatrixOrder::ColumnMajor;
    }

    void GLRenderAPI::Destroy()
    {
        TextureManager::ShutDown();
        RenderStateManager::ShutDown();
        HardwareBufferManager::ShutDown();

        RenderAPI::Destroy();
    }

    void GLRenderAPI::SetGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState)
    {
        // TODO
    }

    void GLRenderAPI::SetGpuParams(const SPtr<GpuParams>& gpuParams)
    {
        // TODO
    }

    void GLRenderAPI::SetViewport(const Rect2& area)
    {
        // TODO
    }

    void GLRenderAPI::SetScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
    {
        // TODO
    }

    void GLRenderAPI::SetStencilRef(UINT32 value)
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

    void GLRenderAPI::SetVertexDeclaration(const SPtr<VertexDeclaration>& vertexDeclaration)
    {
        // TODO
    }

    void GLRenderAPI::SetDrawOperation(DrawOperationType op)
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

    void GLRenderAPI::SetRenderTarget(const SPtr<RenderTarget>& target, UINT32 readOnlyFlags)
    {
        // TODO
    }

    void GLRenderAPI::ClearRenderTarget(UINT32 buffers, const Color& color, float depth, UINT16 stencil, UINT8 targetMask)
    {
        // TODO
    }

    void GLRenderAPI::ClearViewport(UINT32 buffers, const Color& color, float depth, UINT16 stencil, UINT8 targetMask)
    {
        // TODO
    }

    void GLRenderAPI::ConvertProjectionMatrix(const Matrix4& matrix, Matrix4& dest)
    {
        dest = matrix;
    }

    GpuParamBlockDesc GLRenderAPI::GenerateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params)
    {
        GpuParamBlockDesc block;
        block.BlockSize = 0;
        block.IsShareable = true;
        block.Name = name;
        block.Slot = 0;
        block.Set = 0;

        for (auto& param : params)
        {
            UINT32 size;

            if (param.Type == GPDT_STRUCT)
            {
                // Structs are always aligned and rounded up to vec4
                size = Math::DivideAndRoundUp(param.ElementSize, 16U) * 4;
                block.BlockSize = Math::DivideAndRoundUp(block.BlockSize, 4U) * 4;
            }
            else
                size = GLGLSLParamParser::CalcInterfaceBlockElementSizeAndOffset(param.Type, param.ArraySize, block.BlockSize);

            if (param.ArraySize > 1)
            {
                param.ElementSize = size;
                param.ArrayElementStride = size;
                param.CpuMemOffset = block.BlockSize;
                param.GpuMemOffset = 0;

                block.BlockSize += size * param.ArraySize;
            }
            else
            {
                param.ElementSize = size;
                param.ArrayElementStride = size;
                param.CpuMemOffset = block.BlockSize;
                param.GpuMemOffset = 0;

                block.BlockSize += size;
            }

            param.ParamBlockSlot = 0;
            param.ParamBlockSet = 0;
        }

        // Constant buffer size must always be a multiple of 16
        if (block.BlockSize % 4 != 0)
            block.BlockSize += (4 - (block.BlockSize % 4));

        return block;
    }
}
