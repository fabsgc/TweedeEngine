#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuProgram.h"

namespace te
{
    /**	Abstraction of a DirectX 11 shader object. */
	class GLGpuProgram : public GpuProgram
	{
	public:
		virtual ~GLGpuProgram();

		/**	Returns compiled shader microcode. */
		const DataBlob& GetMicroCode() const { return _bytecode->Instructions; }

		/**	Returns unique GPU program ID. */
		UINT32 GetProgramId() const { return _programId; }

	protected:
		GLGpuProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

		/** @copydoc GpuProgram::Initialize */
		void Initialize() override;

	protected:
		static UINT32 GlobalProgramId;

		UINT32 _programId = 0;
	};

    /**	Implementation of a DX11 vertex shader. */
    class GLGpuVertexProgram : public GLGpuProgram
    {
    public:
        ~GLGpuVertexProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuVertexProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /**	Implementation of a DX11 pixel shader. */
    class GLGpuFragmentProgram : public GLGpuProgram
    {
    public:
        ~GLGpuFragmentProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuFragmentProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /**	Implementation of a DX11 domain shader. */
    class GLGpuDomainProgram : public GLGpuProgram
    {
    public:
        ~GLGpuDomainProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuDomainProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /**	Implementation of a DX11 hull shader. */
    class GLGpuHullProgram : public GLGpuProgram
    {
    public:
        ~GLGpuHullProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuHullProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /**	Implementation of a DX11 geometry shader. */
    class GLGpuGeometryProgram : public GLGpuProgram
    {
    public:
        ~GLGpuGeometryProgram();

    protected:
        friend class GLGLSLProgramFactory;

        GLGpuGeometryProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);
    };

    /** Identifier of the compiler used for compiling OpenGL GPU programs. */
    static constexpr const char* OPENGL_COMPILER_ID = "OpenGL";
}
