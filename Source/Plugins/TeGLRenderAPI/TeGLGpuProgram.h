#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuProgram.h"

namespace te
{
    /** Abstraction of a DirectX 11 shader object. */
    class GLGpuProgram : public GpuProgram
    {
    public:
        virtual ~GLGpuProgram();

        /** Returns compiled shader microcode. */
        const DataBlob& GetMicroCode() const { return _bytecode->Instructions; }

        /** @copydoc GpuProgram::isSupported */
        bool IsSupported() const override;

        /**	Gets internal OpenGL handle to the program. */
        GLuint GetGLHandle() const { return _GLHandle; }

        /** Returns unique GPU program ID. */
        UINT32 GetProgramId() const { return _programId; }

    protected:
        GLGpuProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc GpuProgram::Initialize */
        void Initialize() override;

    protected:
        static UINT32 GlobalProgramId;

        UINT32 _programId = 0;
        GLuint _GLHandle = 0;

        static UINT32 sVertexShaderCount;
        static UINT32 sPixelShaderCount;
        static UINT32 sGeometryShaderCount;
        static UINT32 sHullShaderCount;
        static UINT32 sDomainShaderCount;
        static UINT32 sComputeShaderCount;
    };

    /** Implementation of a OpenGL vertex shader. */
    class GLGpuVertexProgram : public GLGpuProgram
    {
    public:
        virtual ~GLGpuVertexProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuVertexProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /** Implementation of a OpenGL fragment shader. */
    class GLGpuPixelProgram : public GLGpuProgram
    {
    public:
        virtual ~GLGpuPixelProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuPixelProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /** Implementation of a OpenGL domain shader. */
    class GLGpuDomainProgram : public GLGpuProgram
    {
    public:
        virtual ~GLGpuDomainProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuDomainProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /** Implementation of a OpenGL hull shader. */
    class GLGpuHullProgram : public GLGpuProgram
    {
    public:
        virtual ~GLGpuHullProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuHullProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /** Implementation of a OpenGL geometry shader. */
    class GLGpuGeometryProgram : public GLGpuProgram
    {
    public:
        virtual ~GLGpuGeometryProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuGeometryProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /** Implementation of a OpenGL compute shader. */
    class GLGpuComputeProgram : public GLGpuProgram
    {
    public:
        virtual ~GLGpuComputeProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuComputeProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /** Identifier of the compiler used for compiling OpenGL GPU programs. */
    static constexpr const char* OPENGL_COMPILER_ID = "OpenGL";

    // TODO
}
