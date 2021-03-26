#include "TeGLRenderAPI.h"
#include "Image/TeTextureManager.h"
#include "TeGLTextureManager.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "TeGLRenderStateManager.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "TeGLHardwareBufferManager.h"
#include "TeGLGLSLParamParser.h"
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

        std::cout << context.get() << std::endl;

        // Set primary context as active
        if (_currentContext)
            _currentContext->SetCurrent(*window);

        // Setup GLSupport
        _GLSupport->InitializeExtensions();

        window->SetVSync(windowDesc.Vsync);

        _GLInitialised = true;

        return window;
    }

    void GLRenderAPI::Initialize()
    {
        _GLSupport->Start();
        _videoModeInfo = _GLSupport->GetVideoModeInfo();

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

        InitFromCaps(_capabilities);

        glFrontFace(GL_CW);
        TE_CHECK_GL_ERROR();

        // Ensure cubemaps are filtered across seams
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        TE_CHECK_GL_ERROR();

        // TODO
    }

    void GLRenderAPI::InitCapabilities(RenderAPICapabilities& caps) const
    {
        Vector<String> tokens = Split(_GLSupport->GetGLVersion(), '.');

        // TODO init opengl

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

        // TODO init opengl
    }

    void GLRenderAPI::InitFromCaps(RenderAPICapabilities* caps)
    {
        // TODO init opengl
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
        /*glDepthMask(_depthWrite);
        TE_CHECK_GL_ERROR();

        for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
            glColorMask(_colorWrite[i][0], _colorWrite[i][1], _colorWrite[i][2], _colorWrite[i][3]);
        TE_CHECK_GL_ERROR();

        glStencilMask(_stencilWriteMask);
        TE_CHECK_GL_ERROR();*/

        // TODO switch context opengl
    }

    void GLRenderAPI::Destroy()
    {
        if (_GLSupport)
            _GLSupport->Stop();

        TextureManager::ShutDown();
        RenderStateManager::ShutDown();
        HardwareBufferManager::ShutDown();

        _GLInitialised = false;

        _currentContext = nullptr;
        _mainContext = nullptr;

        if (_GLSupport)
            te_delete(_GLSupport);

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
