#include "TeD3D11GpuProgram.h"
#include "TeD3D11Device.h"
//#include "RenderAPI/TeGpuParams.h"
#include "TeD3D11RenderAPI.h"
#include "RenderAPI/TeGpuProgramManager.h"
//#include "RenderAPI/TeHardwareBufferManager.h"
//#include "TeD3D11HLSLParamParser.h"
#include <regex>

namespace te
{
	UINT32 D3D11GpuProgram::GlobalProgramId = 0;

	D3D11GpuProgram::D3D11GpuProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
		: GpuProgram(desc, deviceMask)
	{
		assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
	}

	D3D11GpuProgram::~D3D11GpuProgram()
	{
		_inputDeclaration = nullptr;
	}

	void D3D11GpuProgram::Initialize()
	{
        if (!IsSupported())
        {
            _status.Successful = false;
            _status.Message = "Specified program is not supported by the current render system.";

            GpuProgram::Initialize();
            return;
        }

        if (!_bytecode || _bytecode->CompilerId != DIRECTX_COMPILER_ID)
        {
            GPU_PROGRAM_DESC desc;
            desc.Type = _type;
            desc.EntryPoint = _entryPoint;
            desc.Source = _source;
            desc.Language = "hlsl";

            _bytecode = CompileBytecode(desc);
        }

        _status.Message = _bytecode->Message;
        _status.Successful = _bytecode->Instructions.Data != nullptr;

        if (_status.Successful)
        {
            _parametersDesc = _bytecode->ParamDesc;

            D3D11RenderAPI* rapi = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
            LoadFromMicrocode(rapi->GetPrimaryDevice(), _bytecode->Instructions);

            if (_type == GPT_VERTEX_PROGRAM)
            {
                //_inputDeclaration = HardwareBufferManager::Instance().CreateVertexDeclaration(_bytecode->VertexInput); TODO
            }
        }

        _programId = GlobalProgramId++;

        GpuProgram::Initialize();
    }

    D3D11GpuVertexProgram::D3D11GpuVertexProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : D3D11GpuProgram(desc, deviceMask)
        , _vertexShader(nullptr)
    { }

    D3D11GpuVertexProgram::~D3D11GpuVertexProgram()
    {
        SAFE_RELEASE(_vertexShader);
    }

    void D3D11GpuVertexProgram::LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode)
    {
        HRESULT hr = device.GetD3D11Device()->CreateVertexShader(
            microcode.Data, microcode.Size, device.GetClassLinkage(), &_vertexShader);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create D3D11 vertex shader from microcode\nError Description:" + errorDescription, __FILE__, __LINE__);
        }
    }

    ID3D11VertexShader* D3D11GpuVertexProgram::GetVertexShader() const
    {
        return _vertexShader;
    }

    D3D11GpuFragmentProgram::D3D11GpuFragmentProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : D3D11GpuProgram(desc, deviceMask)
        , _pixelShader(nullptr)
    { }

    D3D11GpuFragmentProgram::~D3D11GpuFragmentProgram()
    {
        SAFE_RELEASE(_pixelShader);
    }

    void D3D11GpuFragmentProgram::LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode)
    {
        HRESULT hr = device.GetD3D11Device()->CreatePixelShader(
            microcode.Data, microcode.Size, device.GetClassLinkage(), &_pixelShader);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create D3D11 pixel shader from microcode.\nError Description:" + errorDescription, __FILE__, __LINE__);
        }
    }

    ID3D11PixelShader* D3D11GpuFragmentProgram::GetPixelShader() const
    {
        return _pixelShader;
    }

    D3D11GpuGeometryProgram::D3D11GpuGeometryProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : D3D11GpuProgram(desc, deviceMask)
        , _geometryShader(nullptr)
    { }

    D3D11GpuGeometryProgram::~D3D11GpuGeometryProgram()
    {
        SAFE_RELEASE(_geometryShader);
    }

    void D3D11GpuGeometryProgram::LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode)
    {
        HRESULT hr = device.GetD3D11Device()->CreateGeometryShader(
            microcode.Data, microcode.Size, device.GetClassLinkage(), &_geometryShader);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create D3D11 geometry shader from microcode.\nError Description:" + errorDescription, __FILE__, __LINE__);
        }
    }

    ID3D11GeometryShader* D3D11GpuGeometryProgram::GetGeometryShader() const
    {
        return _geometryShader;
    }

    D3D11GpuDomainProgram::D3D11GpuDomainProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : D3D11GpuProgram(desc, deviceMask)
        , _domainShader(nullptr)
    { }

    D3D11GpuDomainProgram::~D3D11GpuDomainProgram()
    {
        SAFE_RELEASE(_domainShader);
    }

    void D3D11GpuDomainProgram::LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode)
    {
        HRESULT hr = device.GetD3D11Device()->CreateDomainShader(
            microcode.Data, microcode.Size, device.GetClassLinkage(), &_domainShader);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create D3D11 domain shader from microcode.\nError Description:" + errorDescription, __FILE__, __LINE__);
        }
    }

    ID3D11DomainShader* D3D11GpuDomainProgram::GetDomainShader() const
    {
        return _domainShader;
    }

    D3D11GpuHullProgram::D3D11GpuHullProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
        : D3D11GpuProgram(desc, deviceMask)
        , _hullShader(nullptr)
    { }

    D3D11GpuHullProgram::~D3D11GpuHullProgram()
    {
        SAFE_RELEASE(_hullShader);
    }

    void D3D11GpuHullProgram::LoadFromMicrocode(D3D11Device& device, const DataBlob& microcode)
    {
        // Create the shader
        HRESULT hr = device.GetD3D11Device()->CreateHullShader(
            microcode.Data, microcode.Size, device.GetClassLinkage(), &_hullShader);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create D3D11 hull shader from microcode.\nError Description:" + errorDescription, __FILE__, __LINE__);
        }
    }

    ID3D11HullShader* D3D11GpuHullProgram::GetHullShader() const
    {
        return _hullShader;
    }
}
