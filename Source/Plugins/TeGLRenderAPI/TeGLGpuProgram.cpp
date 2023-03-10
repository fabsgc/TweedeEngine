#include "TeGLGpuProgram.h"
//#include "RenderAPI/TeGpuParams.h"
#include "TeGLRenderAPI.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "RenderAPI/TeHardwareBufferManager.h"
#include <regex>
#include <filesystem>

namespace te
{
    UINT32 GLGpuProgram::GlobalProgramId = 0;

    UINT32 GLGpuProgram::sVertexShaderCount = 0;
    UINT32 GLGpuProgram::sPixelShaderCount = 0;
    UINT32 GLGpuProgram::sGeometryShaderCount = 0;
    UINT32 GLGpuProgram::sDomainShaderCount = 0;
    UINT32 GLGpuProgram::sHullShaderCount = 0;
    UINT32 GLGpuProgram::sComputeShaderCount = 0;

    GLGpuProgram::GLGpuProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : GpuProgram(desc, deviceMask)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on OpenGL.");
    }

    GLGpuProgram::~GLGpuProgram()
    {
        _inputDeclaration = nullptr;
    }

    void GLGpuProgram::Initialize()
    {
        if (!IsSupported())
        {
            _status.Successful = false;
            _status.Message = "Specified program is not supported by the current render system.";

            GpuProgram::Initialize();
            return;
        }

        if (!_bytecode || _bytecode->CompilerId != OPENGL_COMPILER_ID)
        {
            GPU_PROGRAM_DESC desc;
            desc.Type = _type;
            desc.EntryPoint = _entryPoint;
            desc.Source = _source;
            desc.Language = "glsl";
             desc.IncludePath = _includePath;
            desc.FilePath = _filePath;
            desc.Variation = _variation;

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            auto path = std::filesystem::absolute(desc.FilePath);
            _debugName = path.filename().generic_string();
#endif

            _bytecode = CompileBytecode(desc);
        }

        _status.Message = _bytecode->Message;
        _status.Successful = _bytecode->Instructions.Data != nullptr;

        if (_status.Successful) 
        {
            _parametersDesc = _bytecode->ParamDesc;

            GLRenderAPI* rapi = static_cast<GLRenderAPI*>(RenderAPI::InstancePtr());
            // LoadFromMicrocode(rapi->GetPrimaryDevice(), _bytecode->Instructions); TODO

            if (_type == GPT_VERTEX_PROGRAM)
            {
                _inputDeclaration = HardwareBufferManager::Instance().CreateVertexDeclaration(_bytecode->VertexInput);
            }
        }
        else
        {
            // TODO to remove
            if (_type == GPT_VERTEX_PROGRAM)
            {
                _inputDeclaration = HardwareBufferManager::Instance().CreateVertexDeclaration(_bytecode->VertexInput);
            }
        }

        _programId = GlobalProgramId++;

        GpuProgram::Initialize();
    }

    bool GLGpuProgram::IsSupported() const
    {
        RenderAPI* rapi = RenderAPI::InstancePtr();
        const RenderAPICapabilities& caps = rapi->GetCapabilities(0);

        switch (_type)
        {
        case GPT_GEOMETRY_PROGRAM:
#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
            return caps.HasCapability(RSC_GEOMETRY_PROGRAM);
#else
            return false;
#endif
        case GPT_HULL_PROGRAM:
        case GPT_DOMAIN_PROGRAM:
#if TE_OPENGL_4_1 || TE_OPENGLES_3_2
            return caps.HasCapability(RSC_TESSELLATION_PROGRAM);
#else
            return false;
#endif
        case GPT_COMPUTE_PROGRAM:
#if TE_OPENGL_4_3 || TE_OPENGLES_3_1
            return caps.HasCapability(RSC_COMPUTE_PROGRAM);
#else
            return false;
#endif
        default:
            return true;
        }
    }

    GLGpuVertexProgram::GLGpuVertexProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : GLGpuProgram(desc, deviceMask)
    { }

    GLGpuVertexProgram::~GLGpuVertexProgram()
    { }

    GLGpuPixelProgram::GLGpuPixelProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : GLGpuProgram(desc, deviceMask)
    { }

    GLGpuPixelProgram::~GLGpuPixelProgram()
    { }

    GLGpuGeometryProgram::GLGpuGeometryProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : GLGpuProgram(desc, deviceMask)
    { }

    GLGpuGeometryProgram::~GLGpuGeometryProgram()
    { }

    GLGpuDomainProgram::GLGpuDomainProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : GLGpuProgram(desc, deviceMask)
    { }

    GLGpuDomainProgram::~GLGpuDomainProgram()
    { }

    GLGpuHullProgram::GLGpuHullProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : GLGpuProgram(desc, deviceMask)
    { }

    GLGpuHullProgram::~GLGpuHullProgram()
    { }

    GLGpuComputeProgram::GLGpuComputeProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : GLGpuProgram(desc, deviceMask)
    { }

    GLGpuComputeProgram::~GLGpuComputeProgram()
    { }
}
