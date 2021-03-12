#include "TeD3D11HLSLProgramFactory.h"
#include "TeD3D11GpuProgram.h"
#include "RenderAPI/TeVertexDeclaration.h"
#include "TeD3D11HLSLParamParser.h"
#include "RenderAPI/TeGpuParamDesc.h"
#include "Utility/TeDataStream.h"
#include <regex>

namespace te
{
    SPtr<GpuProgram> D3D11HLSLProgramFactory::Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<GpuProgram> gpuProgram;

        switch (desc.Type)
        {
        case GPT_VERTEX_PROGRAM:
            gpuProgram = te_core_ptr<D3D11GpuVertexProgram>(new (te_allocate<D3D11GpuVertexProgram>())
                D3D11GpuVertexProgram(desc, deviceMask));
            break;
        case GPT_PIXEL_PROGRAM:
            gpuProgram = te_core_ptr<D3D11GpuPixelProgram>(new (te_allocate<D3D11GpuPixelProgram>())
                D3D11GpuPixelProgram(desc, deviceMask));
            break;
        case GPT_HULL_PROGRAM:
            gpuProgram = te_core_ptr<D3D11GpuHullProgram>(new (te_allocate<D3D11GpuHullProgram>())
                D3D11GpuHullProgram(desc, deviceMask));
            break;
        case GPT_DOMAIN_PROGRAM:
            gpuProgram = te_core_ptr<D3D11GpuDomainProgram>(new (te_allocate<D3D11GpuDomainProgram>())
                D3D11GpuDomainProgram(desc, deviceMask));
            break;
        case GPT_GEOMETRY_PROGRAM:
            gpuProgram = te_core_ptr<D3D11GpuGeometryProgram>(new (te_allocate<D3D11GpuGeometryProgram>())
                D3D11GpuGeometryProgram(desc, deviceMask));
            break;
        case GPT_COMPUTE_PROGRAM:
            gpuProgram = te_core_ptr<D3D11GpuComputeProgram>(new (te_allocate<D3D11GpuComputeProgram>())
                D3D11GpuComputeProgram(desc, deviceMask));
            break;
        }

        if (gpuProgram != nullptr)
        {
            gpuProgram->SetThisPtr(gpuProgram);
        }

        return gpuProgram;
    }

    SPtr<GpuProgram> D3D11HLSLProgramFactory::Create(GpuProgramType type, GpuDeviceFlags deviceMask)
    {
        SPtr<GpuProgram> gpuProgram;

        GPU_PROGRAM_DESC desc;
        desc.Type = type;

        switch (desc.Type)
        {
        case GPT_VERTEX_PROGRAM:
            gpuProgram = te_shared_ptr<D3D11GpuVertexProgram>(new (te_allocate<D3D11GpuVertexProgram>())
                D3D11GpuVertexProgram(desc, deviceMask));
            break;
        case GPT_PIXEL_PROGRAM:
            gpuProgram = te_shared_ptr<D3D11GpuPixelProgram>(new (te_allocate<D3D11GpuPixelProgram>())
                D3D11GpuPixelProgram(desc, deviceMask));
            break;
        case GPT_HULL_PROGRAM:
            gpuProgram = te_shared_ptr<D3D11GpuHullProgram>(new (te_allocate<D3D11GpuHullProgram>())
                D3D11GpuHullProgram(desc, deviceMask));
            break;
        case GPT_DOMAIN_PROGRAM:
            gpuProgram = te_shared_ptr<D3D11GpuDomainProgram>(new (te_allocate<D3D11GpuDomainProgram>())
                D3D11GpuDomainProgram(desc, deviceMask));
            break;
        case GPT_GEOMETRY_PROGRAM:
            gpuProgram = te_shared_ptr<D3D11GpuGeometryProgram>(new (te_allocate<D3D11GpuGeometryProgram>())
                D3D11GpuGeometryProgram(desc, deviceMask));
            break;
        case GPT_COMPUTE_PROGRAM:
            gpuProgram = te_shared_ptr<D3D11GpuComputeProgram>(new (te_allocate<D3D11GpuComputeProgram>())
                D3D11GpuComputeProgram(desc, deviceMask));
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
            String result = results[1].str();
            return strtol(result.c_str(), nullptr, 10) - 1;
        }

        return 0;
    }

    ID3DBlob* D3D11HLSLProgramFactory::OpenMicroCode(const std::filesystem::path& path)
    {
        ID3DBlob* microcode = nullptr;
        D3DReadFileToBlob(path.generic_wstring().c_str(), &microcode);
        return microcode;
    }

    SPtr<GpuProgramBytecode> D3D11HLSLProgramFactory::CompileBytecode(const GPU_PROGRAM_DESC& desc)
    {
        SPtr<GpuProgramBytecode> bytecode = nullptr;
        ID3DBlob* microcode = nullptr;
        ID3DBlob* messages = nullptr;
        D3D11HLSLInclude* include = nullptr;

        std::filesystem::path compiledShaderPath = std::filesystem::current_path();
        compiledShaderPath.append("Shader_" + desc.FileName + ".blob");

        if (std::filesystem::exists(compiledShaderPath))
        {
            microcode = OpenMicroCode(compiledShaderPath);
        }

        if (!microcode)
        {

            String hlslProfile;
            switch (desc.Type)
            {
            case GPT_VERTEX_PROGRAM:
                hlslProfile = "vs_5_0";
                break;
            case GPT_PIXEL_PROGRAM:
                hlslProfile = "ps_5_0";
                break;
            case GPT_GEOMETRY_PROGRAM:
                hlslProfile = "gs_5_0";
                break;
            case GPT_HULL_PROGRAM:
                hlslProfile = "hs_5_0";
                break;
            case GPT_DOMAIN_PROGRAM:
                hlslProfile = "ds_5_0";
                break;
            case GPT_COMPUTE_PROGRAM:
                hlslProfile = "cs_5_0";
                break;
            default:
                break;
            }

            UINT compileFlags = 0;

#if defined(TE_DEBUG_MODE)
            compileFlags |= D3DCOMPILE_DEBUG;
            compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
            compileFlags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
            compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
            compileFlags |= D3DCOMPILE_PARTIAL_PRECISION;
#endif

            compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

            const String& source = desc.Source;
            const String& entryPoint = desc.EntryPoint;

            if (desc.IncludePath != "")
                include = te_new<D3D11HLSLInclude>(desc.IncludePath);

            const D3D_SHADER_MACRO defines[] =
            {
                { "HLSL", "1" },
                { nullptr, nullptr }
            };

            HRESULT hr = D3DCompile(
                source.c_str(),		// [in] Pointer to the shader in memory.
                source.size(),		// [in] Size of the shader in memory.
                nullptr,			// [in] The name of the file that contains the shader code.
                defines,			// [in] Optional. Pointer to a NULL-terminated array of macro definitions.
                                    //		See D3D_SHADER_MACRO. If not used, set this to NULL.
                include,			// [in] Optional. Pointer to an ID3DInclude Interface interface for handling include files.
                                    //		Setting this to NULL will cause a compile error if a shader contains a #include.
                entryPoint.c_str(),	// [in] Name of the shader-entrypoint function where shader execution begins.
                hlslProfile.c_str(),// [in] A string that specifies the shader model; can be any profile in shader model 4 or higher.
                compileFlags,		// [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
                0,					// [in] Effect compile flags
                &microcode,			// [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as
                                    //		 any embedded debug and symbol-table information.
                &messages			// [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings
                                    //		 that occurred during compilation. These errors and warnings are identical to the
                                    //		 debug output from a debugger.
            );

            String compileMessage;
            if (messages != nullptr)
            {
                const char* message = static_cast<const char*>(messages->GetBufferPointer());
                UINT32 lineIdx = ParseErrorMessage(message);

                Vector<String> sourceLines = Util::Split(source, "\n");
                String sourceLine;
                if (lineIdx < sourceLines.size())
                    sourceLine = sourceLines[lineIdx];

                compileMessage =
                    String(message) + "\n" +
                    "\n" +
                    "Line " + ToString(lineIdx) + ": " + sourceLine;

                SAFE_RELEASE(messages);

                TE_ASSERT_ERROR(false, "Can't compile shader file : " + compileMessage);
            }

            bytecode = te_shared_ptr_new<GpuProgramBytecode>();
            bytecode->CompilerId = DIRECTX_COMPILER_ID;
            bytecode->Message = compileMessage;

            if (FAILED(hr))
            {
                SAFE_RELEASE(microcode);
                return bytecode;
            }

            if (microcode != nullptr)
            {
                if (std::filesystem::exists(compiledShaderPath) == false)
                {
                    D3DWriteBlobToFile(microcode, compiledShaderPath.generic_wstring().c_str(), false);
                }
            }

            if (include)
                te_delete(include);
        }
        else
        {
            bytecode = te_shared_ptr_new<GpuProgramBytecode>();
            bytecode->CompilerId = DIRECTX_COMPILER_ID;
        }

        if (microcode != nullptr)
        {
            bytecode->Instructions.Size = (UINT32)microcode->GetBufferSize();
            bytecode->Instructions.Data = (UINT8*)te_allocate(bytecode->Instructions.Size);

            memcpy(bytecode->Instructions.Data, microcode->GetBufferPointer(), bytecode->Instructions.Size);

            D3D11HLSLParamParser parser;
            bytecode->ParamDesc = te_shared_ptr_new<GpuParamDesc>();

            if (desc.Type == GPT_VERTEX_PROGRAM || desc.Type == GPT_PIXEL_PROGRAM)
                parser.Parse(microcode, desc.Type, *bytecode->ParamDesc, &bytecode->VertexInput);
            else
                parser.Parse(microcode, desc.Type, *bytecode->ParamDesc, nullptr);
        }

        SAFE_RELEASE(microcode);
        return bytecode;
    }
}
