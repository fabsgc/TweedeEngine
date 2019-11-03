#include "TeD3D11Driver.h"

namespace te
{
    D3D11Driver::D3D11Driver(const D3D11Driver& ob)
    {
        _adapterNumber = ob._adapterNumber;
        _adapterIdentifier = ob._adapterIdentifier;
        _DXGIAdapter = ob._DXGIAdapter;

        if (_DXGIAdapter)
        {
            _DXGIAdapter->AddRef();
        }

        Construct();
    }

    D3D11Driver::D3D11Driver(UINT32 adapterNumber, IDXGIAdapter* pDXGIAdapter)
    {
        _adapterNumber = adapterNumber;
        _DXGIAdapter = pDXGIAdapter;

        if (_DXGIAdapter)
        {
            _DXGIAdapter->AddRef();
        }

        pDXGIAdapter->GetDesc(&_adapterIdentifier);

        Construct();
    }

    D3D11Driver::~D3D11Driver()
    {
        SAFE_RELEASE(_DXGIAdapter);
    }

    void D3D11Driver::Construct()
    {

    }
}