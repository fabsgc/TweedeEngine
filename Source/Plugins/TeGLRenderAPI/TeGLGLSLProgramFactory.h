#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuProgramManager.h"

namespace te
{
    /** Handles creation of OpenGL GPU programs. */
    class GLGLSLProgramFactory : public GpuProgramFactory
    {
    public:
        GLGLSLProgramFactory() = default;
        virtual ~GLGLSLProgramFactory() = default;

        /** @copydoc GpuProgramFactory::Create(const GPU_PROGRAM_DESC&, GpuDeviceFlags) */
        SPtr<GpuProgram> Create(const GPU_PROGRAM_DESC & desc, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc GpuProgramFactory::Create(GpuProgramType, GpuDeviceFlags) */
        SPtr<GpuProgram> Create(GpuProgramType type, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc GpuProgramFactory::CompileBytecode(const GPU_PROGRAM_DESC&) */
        SPtr<GpuProgramBytecode> CompileBytecode(const GPU_PROGRAM_DESC & desc) override;
    };
}
