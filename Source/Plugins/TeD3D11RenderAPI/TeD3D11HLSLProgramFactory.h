#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "TeD3D11HLSLInclude.h"

#include <filesystem>

namespace te
{

    /** Handles creation of DirectX 11 HLSL GPU programs. */
    class D3D11HLSLProgramFactory : public GpuProgramFactory
    {
    public:
        D3D11HLSLProgramFactory() = default;
        virtual ~D3D11HLSLProgramFactory() = default;

        /** @copydoc GpuProgramFactory::Create(const GPU_PROGRAM_DESC&, GpuDeviceFlags) */
        SPtr<GpuProgram> Create(const GPU_PROGRAM_DESC & desc, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc GpuProgramFactory::Create(GpuProgramType, GpuDeviceFlags) */
        SPtr<GpuProgram> Create(GpuProgramType type, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc GpuProgramFactory::CompileBytecode(const GPU_PROGRAM_DESC&) */
        SPtr<GpuProgramBytecode> CompileBytecode(const GPU_PROGRAM_DESC & desc) override;

    private:
        /** If shader is already compiled, we open it instead compile it */
        ID3DBlob* OpenMicroCode(const std::filesystem::path& path);
    };
}
