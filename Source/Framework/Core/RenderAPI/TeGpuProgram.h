#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Utility/TeDataBlob.h"
#include "TeVertexDeclaration.h"

namespace te
{
    struct GpuProgramBytecode;

    /** Descriptor structure used for initialization of a GpuProgram. */
    struct TE_CORE_EXPORT GPU_PROGRAM_DESC
    {
        String FilePath; /**< FilePath used to know if shader is already built or not */
        String Source; /**< Source code to compile the program from. */
        String EntryPoint; /**< Name of the entry point function, for example "main". */
        String Language; /**< Language the source is written in, for example "hlsl" or "glsl". */
        String IncludePath; /**< For hlsl, you can specify an include path here */
        GpuProgramType Type = GPT_VERTEX_PROGRAM; /**< Type of the program, for example vertex or pixel. */
        bool RequiresAdjacency = false; /**< If true then adjacency information will be provided when rendering. */

        /**
         * Optional intermediate version of the GPU program. Can significantly speed up GPU program compilation/creation
         * when supported by the render backend. Call GpuProgram::CompileBytecode to generate it.
         */
        SPtr<GpuProgramBytecode> Bytecode;
    };

    /**
     * A GPU program compiled to an intermediate bytecode format, as well as any relevant meta-data that could be
     * extracted from that format.
     */
    struct TE_CORE_EXPORT GpuProgramBytecode
    {
        ~GpuProgramBytecode();

        /** Instructions (compiled code) for the GPU program. Contains no data if compilation was not succesful. */
        DataBlob Instructions;

        /** Reflected information about GPU program parameters. */
        SPtr<GpuParamDesc> ParamDesc;

        /** Input parameters for a vertex GPU program. */
        Vector<VertexElement> VertexInput;

        /** Message output during the compilation process. Includes errors in case compilation failed. */
        String Message;

        /** Identifier of the compiler that compiled the bytecode. */
        String CompilerId;

        /** Version of the compiler that compiled the bytecode. */
        UINT32 CompilerVersion = 0;
    };

    /**
     * Contains a GPU program such as vertex or pixel program which gets compiled from the provided source code.
     */
    class TE_CORE_EXPORT GpuProgram : public CoreObject
    {
    public:
        /** Information returned when compiling a GPU program. */
        struct CompilationStatus
        {
            bool Successful = false;
            String Message;
        };
    
    public:
        virtual ~GpuProgram();

        /** Returns whether this program can be supported on the current renderer and hardware. */
        virtual bool IsSupported() const;

        /** Returns true if program was successfully compiled. */
        virtual bool IsCompiled() const { return _status.Successful; }

        /** Returns an error message returned by the compiler, if the compilation failed. */
        String GetCompileErrorMessage() const { return _status.Message; }

        /**
         * Sets whether this geometry program requires adjacency information from the input primitives.
         *
         * @note	Only relevant for geometry programs.
         */
        virtual void SetAdjacencyInfoRequired(bool required) { _needsAdjacencyInfo = required; }

        /**
         * Returns whether this geometry program requires adjacency information from the input primitives.
         *
         * @note	Only relevant for geometry programs.
         */
        virtual bool IsAdjacencyInfoRequired() const { return _needsAdjacencyInfo; }

        /**	Type of GPU program (for example pixel, vertex). */
        GpuProgramType GetType() const { return _type; }

        /** Returns description of all parameters in this GPU program. */
        SPtr<GpuParamDesc> GetParamDesc() const { return _parametersDesc; }

        /**	Returns GPU program input declaration. Only relevant for vertex programs. */
        SPtr<VertexDeclaration> GetInputDeclaration() const { return _inputDeclaration; }

        /** Returns the compiled bytecode of this program. */
        SPtr<GpuProgramBytecode> GetBytecode() const { return _bytecode; }

        /**
         * Creates a new GPU program using the provided source code. If compilation fails or program is not supported
         * isCompiled() with return false, and you will be able to retrieve the error message via getCompileErrorMessage().
         *
         * @param[in]	desc		Description of the program to create.
         */
        static SPtr<GpuProgram> Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * Compiles the GPU program to an intermediate bytecode format. The bytecode can be cached and used for
         * quicker compilation/creation of GPU programs.
         */
        static SPtr<GpuProgramBytecode> CompileBytecode(const GPU_PROGRAM_DESC& desc);

    protected:
        friend class GpuProgramManager;

        GpuProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

    protected:
        CompilationStatus _status;

        GpuProgramType _type;
        String _language;
        String _entryPoint;
        String _source;
        String _includePath;
        String _filePath;
        bool _needsAdjacencyInfo;

        SPtr<GpuParamDesc> _parametersDesc;
        SPtr<VertexDeclaration> _inputDeclaration;
        SPtr<GpuProgramBytecode> _bytecode;
    };
}
