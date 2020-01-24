#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuProgramManager.h"

namespace te
{
    /** Handles creation of DirectX 11 HLSL GPU programs. */
    class D3D11HLSLProgramFactory : public GpuProgramFactory
    {
    public:
        D3D11HLSLProgramFactory() = default;
        ~D3D11HLSLProgramFactory() = default;

        /** @copydoc GpuProgramFactory::Create(const GPU_PROGRAM_DESC&, GpuDeviceFlags) */
        SPtr<GpuProgram> Create(const GPU_PROGRAM_DESC & desc, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc GpuProgramFactory::Create(GpuProgramType, GpuDeviceFlags) */
        SPtr<GpuProgram> Create(GpuProgramType type, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc GpuProgramFactory::CompileBytecode(const GPU_PROGRAM_DESC&) */
        SPtr<GpuProgramBytecode> CompileBytecode(const GPU_PROGRAM_DESC & desc) override;
    };
}
