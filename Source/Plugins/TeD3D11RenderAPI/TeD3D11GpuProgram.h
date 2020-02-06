#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuProgram.h"

namespace te
{
    /** Abstraction of a DirectX 11 shader object. */
    class D3D11GpuProgram : public GpuProgram
    {
    public:
        virtual ~D3D11GpuProgram();

        /** Returns compiled shader microcode. */
        const DataBlob& GetMicroCode() const { return _bytecode->Instructions; }

        /** Returns unique GPU program ID. */
        UINT32 GetProgramId() const { return _programId; }

    protected:
        D3D11GpuProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc GpuProgram::Initialize */
        void Initialize() override;

        /** Loads the shader from microcode. */
        virtual void LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode) = 0;

    protected:
        static UINT32 GlobalProgramId;

        UINT32 _programId = 0;
    };

    /** Implementation of a DX11 vertex shader. */
    class D3D11GpuVertexProgram : public D3D11GpuProgram
    {
    public:
        ~D3D11GpuVertexProgram();

        /** Returns internal DX11 vertex shader object. */
        ID3D11VertexShader* GetVertexShader() const;

    protected:
        friend class D3D11HLSLProgramFactory;

        D3D11GpuVertexProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc D3D11GpuProgram::LoadFromMicrocode */
        void LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode) override;

    protected:
        ID3D11VertexShader* _vertexShader;
    };

    /** Implementation of a DX11 pixel shader. */
    class D3D11GpuPixelProgram : public D3D11GpuProgram
    {
    public:
        ~D3D11GpuPixelProgram();

        /** Returns internal DX11 pixel shader object. */
        ID3D11PixelShader* GetPixelShader() const;

    protected:
        friend class D3D11HLSLProgramFactory;

        D3D11GpuPixelProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc D3D11GpuProgram::LoadFromMicrocode */
        void LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode) override;

    protected:
        ID3D11PixelShader* _pixelShader;
    };

    /** Implementation of a DX11 domain shader. */
    class D3D11GpuDomainProgram : public D3D11GpuProgram
    {
    public:
        ~D3D11GpuDomainProgram();

        /** Returns internal DX11 domain shader object. */
        ID3D11DomainShader* GetDomainShader() const;

    protected:
        friend class D3D11HLSLProgramFactory;

        D3D11GpuDomainProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc D3D11GpuProgram::LoadFromMicrocode */
        void LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode) override;

    protected:
        ID3D11DomainShader* _domainShader;
    };

    /** Implementation of a DX11 hull shader. */
    class D3D11GpuHullProgram : public D3D11GpuProgram
    {
    public:
        ~D3D11GpuHullProgram();

        /** Returns internal DX11 hull shader object. */
        ID3D11HullShader* GetHullShader() const;

    protected:
        friend class D3D11HLSLProgramFactory;

        D3D11GpuHullProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc D3D11GpuProgram::LoadFromMicrocode */
        void LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode) override;

    protected:
        ID3D11HullShader* _hullShader;
    };

    /** Implementation of a DX11 geometry shader. */
    class D3D11GpuGeometryProgram : public D3D11GpuProgram
    {
    public:
        ~D3D11GpuGeometryProgram();

        /** Returns internal DX11 geometry shader object. */
        ID3D11GeometryShader* GetGeometryShader() const;

    protected:
        friend class D3D11HLSLProgramFactory;

        D3D11GpuGeometryProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc D3D11GpuProgram::LoadFromMicrocode */
        void LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode) override;

    protected:
        ID3D11GeometryShader* _geometryShader;
    };

    /** Identifier of the compiler used for compiling DirectX 11 GPU programs. */
    static constexpr const char* DIRECTX_COMPILER_ID = "DirectX11";
}
