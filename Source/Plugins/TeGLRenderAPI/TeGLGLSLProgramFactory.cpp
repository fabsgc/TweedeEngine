#include "TeGLGLSLProgramFactory.h"
#include "TeGLGpuProgram.h"
#include "RenderAPI/TeVertexDeclaration.h"
//#include "TeGLGLSLParamParser.h"
#include "RenderAPI/TeGpuParamDesc.h"
#include <regex>

namespace te
{
    SPtr<GpuProgram> GLGLSLProgramFactory::Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<GpuProgram> gpuProgram;

        switch (desc.Type)
        {
        case GPT_VERTEX_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuVertexProgram>(new (te_allocate<GLGpuVertexProgram>())
                GLGpuVertexProgram(desc, deviceMask));
            break;
        case GPT_PIXEL_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuFragmentProgram>(new (te_allocate<GLGpuFragmentProgram>())
                GLGpuFragmentProgram(desc, deviceMask));
            break;
        case GPT_HULL_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuHullProgram>(new (te_allocate<GLGpuHullProgram>())
                GLGpuHullProgram(desc, deviceMask));
            break;
        case GPT_DOMAIN_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuDomainProgram>(new (te_allocate<GLGpuDomainProgram>())
                GLGpuDomainProgram(desc, deviceMask));
            break;
        case GPT_GEOMETRY_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuGeometryProgram>(new (te_allocate<GLGpuGeometryProgram>())
                GLGpuGeometryProgram(desc, deviceMask));
            break;
        }

        if (gpuProgram != nullptr)
        {
            gpuProgram->SetThisPtr(gpuProgram);
        }

        return gpuProgram;
    }

    SPtr<GpuProgram> GLGLSLProgramFactory::Create(GpuProgramType type, GpuDeviceFlags deviceMask)
    {
        SPtr<GpuProgram> gpuProgram;

        GPU_PROGRAM_DESC desc;
        desc.Type = type;

        switch (desc.Type)
        {
        case GPT_VERTEX_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuVertexProgram>(new (te_allocate<GLGpuVertexProgram>())
                GLGpuVertexProgram(desc, deviceMask));
            break;
        case GPT_PIXEL_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuFragmentProgram>(new (te_allocate<GLGpuFragmentProgram>())
                GLGpuFragmentProgram(desc, deviceMask));
            break;
        case GPT_HULL_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuHullProgram>(new (te_allocate<GLGpuHullProgram>())
                GLGpuHullProgram(desc, deviceMask));
            break;
        case GPT_DOMAIN_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuDomainProgram>(new (te_allocate<GLGpuDomainProgram>())
                GLGpuDomainProgram(desc, deviceMask));
            break;
        case GPT_GEOMETRY_PROGRAM:
            gpuProgram = te_shared_ptr<GLGpuGeometryProgram>(new (te_allocate<GLGpuGeometryProgram>())
                GLGpuGeometryProgram(desc, deviceMask));
            break;
        }

        if (gpuProgram != nullptr)
        {
            gpuProgram->SetThisPtr(gpuProgram);
        }

        return gpuProgram;
    }

    UINT32 ParseErrorMessage(const char* message)
    {
        if (message == nullptr)
        {
            return 0;
        }

        String pattern = R"(\(([0-9]*),.*\))";
        std::regex regex(pattern);

        std::cmatch results;
        if (std::regex_search(message, results, regex))
        {
            std::string result = results[1].str();
            return strtol(result.c_str(), nullptr, 10) - 1;
        }

        return 0;
    }

    SPtr<GpuProgramBytecode> GLGLSLProgramFactory::CompileBytecode(const GPU_PROGRAM_DESC& desc)
    { 
        SPtr<GpuProgramBytecode> bytecode = te_shared_ptr_new<GpuProgramBytecode>();
        bytecode->CompilerId = OPENGL_COMPILER_ID;
        bytecode->Message = "";

        bytecode->Instructions.Size = (UINT32)0;
        bytecode->Instructions.Data = (UINT8*)nullptr;
        
        return bytecode;
    }
}
