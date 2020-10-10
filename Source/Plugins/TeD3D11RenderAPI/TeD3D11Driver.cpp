#include "TeD3D11Driver.h"
#include "TeD3D11VideoModeInfo.h"

namespace te
{
    D3D11Driver::D3D11Driver(const D3D11Driver& ob)
        : _adapterNumber(ob._adapterNumber)
        , _adapterIdentifier(ob._adapterIdentifier)
        , _DXGIAdapter(ob._DXGIAdapter)
    {
        if (_DXGIAdapter)
            _DXGIAdapter->AddRef();

        Construct();
    }

    D3D11Driver::D3D11Driver(UINT32 adapterNumber, IDXGIAdapter* pDXGIAdapter)
        : _adapterNumber(adapterNumber)
        , _DXGIAdapter(pDXGIAdapter)
    {
        if (_DXGIAdapter)
            _DXGIAdapter->AddRef();

        pDXGIAdapter->GetDesc(&_adapterIdentifier);
        Construct();
    }

    D3D11Driver::~D3D11Driver()
    {
        SAFE_RELEASE(_DXGIAdapter);
    }

    void D3D11Driver::Construct()
    {
        assert(_DXGIAdapter != nullptr);

        UINT32 outputIdx = 0;
        IDXGIOutput* output = nullptr;
        while (_DXGIAdapter->EnumOutputs(outputIdx, &output) != DXGI_ERROR_NOT_FOUND)
        {
            outputIdx++;
            SAFE_RELEASE(output);
        }

        _numOutputs = outputIdx;

        _videoModeInfo = te_shared_ptr_new<D3D11VideoModeInfo>(_DXGIAdapter);
    }

    D3D11Driver& D3D11Driver::operator=(const D3D11Driver& ob)
    {
        _adapterNumber = ob._adapterNumber;
        _adapterIdentifier = ob._adapterIdentifier;

        if (ob._DXGIAdapter)
            ob._DXGIAdapter->AddRef();

        SAFE_RELEASE(_DXGIAdapter);
        _DXGIAdapter = ob._DXGIAdapter;

        return *this;
    }

    String D3D11Driver::GetDriverName() const
    {
        size_t size = wcslen(_adapterIdentifier.Description);
        char* str = (char*)te_allocate((UINT32)(size + 1));

        wcstombs(str, _adapterIdentifier.Description, size);
        str[size] = '\0';
        String Description = str;

        te_free(str);
        return String(Description);
    }

    String D3D11Driver::GetDriverDescription() const
    {
        size_t size = wcslen(_adapterIdentifier.Description);
        char* str = (char*)te_allocate((UINT32)(size + 1));

        wcstombs(str, _adapterIdentifier.Description, size);
        str[size] = '\0';
        String driverDescription = str;

        te_free(str);
        Util::Trim(driverDescription);

        return driverDescription;
    }

    DXGI_OUTPUT_DESC D3D11Driver::GetOutputDesc(UINT32 adapterOutputIdx) const
    {
        DXGI_OUTPUT_DESC desc;

        IDXGIOutput* output = nullptr;
        if (_DXGIAdapter->EnumOutputs(adapterOutputIdx, &output) == DXGI_ERROR_NOT_FOUND)
        {
            TE_ASSERT_ERROR(false, "Cannot find output with the specified index: " + ToString(adapterOutputIdx));
        }

        output->GetDesc(&desc);

        SAFE_RELEASE(output);

        return desc;
    }
}
