#include "TeGLRenderAPI.h"
#include "Image/TeTextureManager.h"
#include "Utility/TePlatformUtility.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "RenderAPI/TeRenderAPICapabilities.h"
#include "TeGLVertexArrayObjectManager.h"
#include "TeGLRenderStateManager.h"
#include "TeGLHardwareBufferManager.h"
#include "TeGLGLSLParamParser.h"
#include "TeGLTextureManager.h"
#include "TeGLContext.h"
#include "TeGLSupport.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include "Win32/TeWin32RenderWindow.h"
#elif TE_PLATFORM == TE_PLATFORM_LINUX
#   include "Linux/TeLinuxRenderWindow.h"
#endif

#if TE_PLATFORM == TE_PLATFORM_WIN32

#include "Win32/TeWin32GLSupport.h"
#include "Win32/TeWin32VideoModeInfo.h"

namespace te
{
    /**	Helper method that returns a platform specific GL support object. */
    GLSupport* GetGLSupport()
    {
        return te_new<Win32GLSupport>();
    }
}

#elif TE_PLATFORM == TE_PLATFORM_LINUX

#include "Linux/TeLinuxGLSupport.h"

namespace te
{
    /**	Helper method that returns a platform specific GL support object. */
    GLSupport* GetGLSupport()
    {
        return te_new<LinuxGLSupport>();
    }
}
#endif

namespace te
{
    const char* te_get_gl_error_string(GLenum errorCode)
    {
        switch (errorCode)
        {
            case GL_INVALID_OPERATION: return "INVALID_OPERATION";
            case GL_INVALID_ENUM: return "INVALID_ENUM";
            case GL_INVALID_VALUE: return "INVALID_VALUE";
            case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
            case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION";
        }

        return nullptr;
    }

    void te_check_gl_error(const char* function, const char* file, INT32 line)
    {
        GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR)
        {
            StringStream errorOutput;
            errorOutput << "OpenGL error in " << function << " [" << file << ":" << ToString(line) << "]:\n";

            while (errorCode != GL_NO_ERROR)
            {
                const char* errorString = te_get_gl_error_string(errorCode);
                if (errorString)
                    errorOutput << "\t - " << errorString;

                errorCode = glGetError();
            }

            TE_DEBUG(errorOutput.str());
        }
    }

#if TE_OPENGL_4_3 || TE_OPENGLES_3_2
    void OpenGlErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar
                *message, GLvoid *userParam);
#endif

    TE_MODULE_STATIC_MEMBER(GLRenderAPI)

    GLRenderAPI::GLRenderAPI()
    {
        // Get our GLSupport
        _GLSupport = te::GetGLSupport();

        _currentContext = 0;
        _mainContext = 0;

        _GLInitialised = false;
    }

    SPtr<RenderWindow> GLRenderAPI::CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc)
    {
        SPtr<RenderWindow> window = nullptr;

#if TE_PLATFORM == TE_PLATFORM_WIN32
        window = te_core_ptr_new<Win32RenderWindow>(windowDesc, *(static_cast<Win32GLSupport*>(_GLSupport)));
#elif TE_PLATFORM == TE_PLATFORM_LINUX
        window = te_core_ptr_new<LinuxRenderWindow>(windowDesc, *(static_cast<LinuxGLSupport*>(_GLSupport)));
#endif

        window->Initialize();

        // Get the context from the window and finish initialization
        SPtr<GLContext> context;
        window->GetCustomAttribute("GLCONTEXT", &context);

        // Set main and current context
        _mainContext = context;
        _currentContext = _mainContext;

        // Set primary context as active
        if (_currentContext)
            _currentContext->SetCurrent(*window);

        // Setup GLSupport
        _GLSupport->InitializeExtensions();

        _numDevices = 1;
        _capabilities = te_newN<RenderAPICapabilities>(_numDevices);
        InitCapabilities(_capabilities[0]);

        InitFromCaps(_capabilities);
        GLVertexArrayObjectManager::StartUp();

        glFrontFace(GL_CW);
        TE_CHECK_GL_ERROR();

        // Ensure cubemaps are filtered across seams
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        TE_CHECK_GL_ERROR();

        GPUInfo gpuInfo;
        gpuInfo.NumGPUs = 1;

        const char* vendor = (const char*)glGetString(GL_VENDOR);
        TE_CHECK_GL_ERROR();

        const char* renderer = (const char*)glGetString(GL_RENDERER);
        TE_CHECK_GL_ERROR();

        if (vendor && renderer)
            gpuInfo.Names[0] = String(vendor) + " " + String(renderer);
        else
            gpuInfo.Names[0] = "Unknown";

        window->SetVSync(windowDesc.Vsync);

        _GLInitialised = true;

        return window;
    }

    void GLRenderAPI::Initialize()
    {
        _GLSupport->Start();
        _videoModeInfo = _GLSupport->GetVideoModeInfo();

        // Create render state manager
        RenderStateManager::StartUp<GLRenderStateManager>();

        // Create hardware buffer manager
        HardwareBufferManager::StartUp<GLHardwareBufferManager>();

        RenderAPI::Initialize();
    }

    void GLRenderAPI::InitCapabilities(RenderAPICapabilities& caps) const
    {
        Vector<String> tokens = Split(_GLSupport->GetGLVersion(), '.');

        DriverVersion driverVersion;
        if (!tokens.empty())
        {
            driverVersion.major = ParseINT32(tokens[0]);
            if (tokens.size() > 1)
                driverVersion.minor = ParseINT32(tokens[1]);
            if (tokens.size() > 2)
                driverVersion.release = ParseINT32(tokens[2]);
        }
        driverVersion.build = 0;

        caps.Driver= driverVersion;
        caps.RenderAPIName = "TeGLRenderAPI";

        const char* deviceName = (const char*)glGetString(GL_RENDERER);
        if(deviceName)
            caps.DeviceName = deviceName;

        const char* vendorName = (const char*)glGetString(GL_VENDOR);
        if (vendorName)
        {
            if (strstr(vendorName, "NVIDIA"))
                caps.DeviceVendor = GPU_NVIDIA;
            else if (strstr(vendorName, "ATI"))
                caps.DeviceVendor = GPU_AMD;
            else if (strstr(vendorName, "AMD"))
                caps.DeviceVendor = GPU_AMD;
            else if (strstr(vendorName, "Intel"))
                caps.DeviceVendor = GPU_INTEL;
            else
                caps.DeviceVendor = GPU_UNKNOWN;
        }

        caps.Convention.UV_YAxis = Conventions::Axis::Up;
        caps.Convention.matrixOrder = Conventions::MatrixOrder::ColumnMajor;
        caps.MinDepth = -1.0f;
        caps.MaxDepth = 1.0f;

        GLint maxOutputVertices;

#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxOutputVertices);
        TE_CHECK_GL_ERROR();
#else
        maxOutputVertices = 0;
#endif

        caps.GeometryProgramNumOutputVertices = maxOutputVertices;

        // Max number of fragment shader textures
        GLint units;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &units);
        TE_CHECK_GL_ERROR();

        caps.NumTextureUnitsPerStage[GPT_PIXEL_PROGRAM] = static_cast<UINT16>(units);

        // Max number of vertex shader textures
        GLint vUnits;
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vUnits);
        TE_CHECK_GL_ERROR();

        caps.NumTextureUnitsPerStage[GPT_VERTEX_PROGRAM] = static_cast<UINT16>(vUnits);

        GLint numUniformBlocks;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &numUniformBlocks);
        TE_CHECK_GL_ERROR();

        caps.NumGpuParamBlockBuffersPerStage[GPT_VERTEX_PROGRAM] = numUniformBlocks;

        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &numUniformBlocks);
        TE_CHECK_GL_ERROR();

        caps.NumGpuParamBlockBuffersPerStage[GPT_PIXEL_PROGRAM] = numUniformBlocks;

        {
            GLint geomUnits;

#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
            glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &geomUnits);
            TE_CHECK_GL_ERROR();
#else
            geomUnits = 0;
#endif

            caps.NumTextureUnitsPerStage[GPT_GEOMETRY_PROGRAM] = static_cast<UINT16>(geomUnits);

#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
            glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &numUniformBlocks);
            TE_CHECK_GL_ERROR();
#else
            numUniformBlocks = 0;
#endif

            caps.NumGpuParamBlockBuffersPerStage[GPT_GEOMETRY_PROGRAM] = numUniformBlocks;
        }

        if (_GLSupport->CheckExtension("GL_ARB_tessellation_shader"))
        {
#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
            caps.SetCapability(RSC_TESSELLATION_PROGRAM);
#endif

#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
            glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS, &numUniformBlocks);
            TE_CHECK_GL_ERROR();
#else
            numUniformBlocks = 0;
#endif

            caps.NumGpuParamBlockBuffersPerStage[GPT_HULL_PROGRAM] = numUniformBlocks;

#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
            glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS, &numUniformBlocks);
            TE_CHECK_GL_ERROR();
#else
            numUniformBlocks = 0;
#endif

            caps.NumGpuParamBlockBuffersPerStage[GPT_DOMAIN_PROGRAM] = numUniformBlocks;
        }

        if (_GLSupport->CheckExtension("GL_ARB_compute_shader"))
        {
#if TE_OPENGL_4_3 || TE_OPENGLES_3_1
            caps.SetCapability(RSC_COMPUTE_PROGRAM);
#endif

            GLint computeUnits;

#if TE_OPENGL_4_3 || TE_OPENGLES_3_1
            glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &computeUnits);
            TE_CHECK_GL_ERROR();
#else
            computeUnits = 0;
#endif

            caps.NumTextureUnitsPerStage[GPT_COMPUTE_PROGRAM] = static_cast<UINT16>(computeUnits);

#if TE_OPENGL_4_3 || TE_OPENGLES_3_1
            glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &numUniformBlocks);
            TE_CHECK_GL_ERROR();
#else
            numUniformBlocks = 0;
#endif

            caps.NumGpuParamBlockBuffersPerStage[GPT_COMPUTE_PROGRAM] = numUniformBlocks;

            // Max number of load-store textures
            GLint lsfUnits;

#if TE_OPENGL_4_2 || TE_OPENGLES_3_1
            glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, &lsfUnits);
            TE_CHECK_GL_ERROR();
#else
            lsfUnits = 0;
#endif

            caps.NumLoadStoreTextureUnitsPerStage[GPT_PIXEL_PROGRAM] = static_cast<UINT16>(lsfUnits);

            GLint lscUnits;

#if TE_OPENGL_4_3 || TE_OPENGLES_3_1
            glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, &lscUnits);
            TE_CHECK_GL_ERROR();
#else
            lscUnits = 0;
#endif

            caps.NumLoadStoreTextureUnitsPerStage[GPT_COMPUTE_PROGRAM] = static_cast<UINT16>(lscUnits);

            GLint combinedLoadStoreTextureUnits;

#if TE_OPENGL_4_2 || TE_OPENGLES_3_1
            glGetIntegerv(GL_MAX_IMAGE_UNITS, &combinedLoadStoreTextureUnits);
            TE_CHECK_GL_ERROR();
#else
            combinedLoadStoreTextureUnits = 0;
#endif

            caps.NumCombinedLoadStoreTextureUnits = static_cast<UINT16>(combinedLoadStoreTextureUnits);
        }

        GLint combinedTexUnits;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combinedTexUnits);
        TE_CHECK_GL_ERROR();

        caps.NumCombinedTextureUnits = static_cast<UINT16>(combinedTexUnits);

        GLint combinedUniformBlockUnits;
        glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &combinedUniformBlockUnits);
        TE_CHECK_GL_ERROR();

        caps.NumCombinedParamBlockBuffers = static_cast<UINT16>(combinedUniformBlockUnits);
        caps.NumMultiRenderTargets = 8;
    }

    void GLRenderAPI::InitFromCaps(RenderAPICapabilities* caps)
    {
        if (caps->RenderAPIName != "TeGLRenderAPI")
            TE_ASSERT_ERROR(false, "Trying to initialize GLRenderAPI from RenderSystemCapabilities that do not support OpenGL");

#if TE_DEBUG_MODE && (TE_OPENGL_4_3 || TE_OPENGLES_3_2)
        if (_GLSupport->CheckExtension("GL_ARB_debug_output"))
        {
            glDebugMessageCallback(&OpenGlErrorCallback, 0);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        }
#endif

        // GPU Program Manager setup
        _GLSLFactory = te_new<GLGLSLProgramFactory>();
        if (caps->IsShaderProfileSupported("glsl")) // Check for most recent GLSL support
            GpuProgramManager::Instance().AddFactory("glsl", _GLSLFactory);

        if (caps->IsShaderProfileSupported("glsl4_1")) // Check for OpenGL 4.1 compatible version
            GpuProgramManager::Instance().AddFactory("glsl4_1", _GLSLFactory);

        _numTextureUnits = caps->NumCombinedTextureUnits;
        _textureInfos = te_newN<TextureInfo>(_numTextureUnits);

        // Create the texture manager for use by others
        TextureManager::StartUp<GLTextureManager>(std::ref(*_GLSupport));
    }

    void GLRenderAPI::SwitchContext(const SPtr<GLContext>& context, const RenderWindow& window)
    {
        // Unbind pipeline and rebind to new context later
        SetGraphicsPipeline(nullptr);

        if (_currentContext)
            _currentContext->EndCurrent();

        _currentContext = context;
        _currentContext->SetCurrent(window);

        // Must reset depth/colour write mask to according with user desired, otherwise, clearFrameBuffer would be wrong
        // because the value we recorded may be different from the real state stored in GL context.
        glDepthMask(_depthWrite);
        TE_CHECK_GL_ERROR();

        for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
            glColorMask(_colorWrite[i][0], _colorWrite[i][1], _colorWrite[i][2], _colorWrite[i][3]);
        TE_CHECK_GL_ERROR();

        glStencilMask(_stencilWriteMask);
        TE_CHECK_GL_ERROR();
    }

    void GLRenderAPI::Destroy()
    {
        if (_GLSupport)
            _GLSupport->Stop();

        // Deleting the GLSL program factory
        if (_GLSLFactory)
        {
            // Remove from manager safely
            GpuProgramManager::Instance().RemoveFactory("glsl");
            GpuProgramManager::Instance().RemoveFactory("glsl4_1");

            te_delete(_GLSLFactory);
            _GLSLFactory = nullptr;
        }

        TextureManager::ShutDown();
        RenderStateManager::ShutDown();
        HardwareBufferManager::ShutDown();
        GLVertexArrayObjectManager::ShutDown();


        for (UINT32 i = 0; i < MAX_VB_COUNT; i++)
            _boundVertexBuffers[i] = nullptr;

        _boundVertexDeclaration = nullptr;
        _boundIndexBuffer = nullptr;

        _currentVertexProgram = nullptr;
        _currentFragmentProgram = nullptr;
        _currentGeometryProgram = nullptr;
        _currentHullProgram = nullptr;
        _currentDomainProgram = nullptr;
        _currentComputeProgram = nullptr;

        _GLInitialised = false;

        if (_currentContext)
            _currentContext->EndCurrent();

        if (_mainContext && _mainContext != _currentContext)
            _mainContext->EndCurrent();

        if(_currentContext != _mainContext)
        {
            _currentContext = nullptr;
            _mainContext = nullptr;
        }
        else
        {
            _currentContext = nullptr;
        }

        if (_GLSupport)
            te_delete(_GLSupport);

        if (_textureInfos != nullptr)
            te_deleteN(_textureInfos, _numTextureUnits);

        RenderAPI::Destroy();
    }

    void GLRenderAPI::SetGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState)
    {
        // TODO
    }

    void GLRenderAPI::SetComputePipeline(const SPtr<ComputePipelineState>& pipelineState)
    {
        // TODO
    }

    void GLRenderAPI::SetGpuParams(const SPtr<GpuParams>& gpuParams, UINT32 gpuParamsBindFlags,
        UINT32 gpuParamsBlockBindFlags, const Vector<String>& paramBlocksToBind)
    {
        // TODO
    }

    void GLRenderAPI::SetViewport(const Rect2& area)
    {
        // TODO
    }

    void GLRenderAPI::SetScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
    {
        _scissorTop = top;
        _scissorBottom = bottom;
        _scissorLeft = left;
        _scissorRight = right;

        if (_scissorEnabled)
            _scissorRectDirty = true;
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

    void GLRenderAPI::DispatchCompute(UINT32 numGroupsX, UINT32 numGroupsY, UINT32 numGroupsZ)
    {
        // TODO
    }

    void GLRenderAPI::SwapBuffers(const SPtr<RenderTarget>& target)
    {
        // Switch context if different from current one
        if (!target->GetProperties().IsWindow)
            return;

        RenderWindow* window = static_cast<RenderWindow*>(target.get());

        SPtr<GLContext> newContext;
        target->GetCustomAttribute("GLCONTEXT", &newContext);
        if (newContext && _currentContext != newContext)
            SwitchContext(newContext, *window);
        else
            _currentContext->SetCurrent(*window);

        target->SwapBuffers();
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

    UINT64 GLRenderAPI::GetGPUMemory()
    {
        return 0;
    }

    UINT64 GLRenderAPI::GetUsedGPUMemory()
    {
        return 0;
    }

#if TE_OPENGL_4_3 || TE_OPENGLES_3_2
    void OpenGlErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
        const GLchar *message, GLvoid *userParam)
    {
        if (type != GL_DEBUG_TYPE_PERFORMANCE && type != GL_DEBUG_TYPE_OTHER)
        {
            TE_ASSERT_ERROR(true, "OpenGL error: " + String(message));
        }
    }
#endif
}
