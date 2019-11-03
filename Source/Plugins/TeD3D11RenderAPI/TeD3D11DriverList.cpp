#include "TeD3D11DriverList.h"
#include "TeD3D11Driver.h"

namespace te
{
    D3D11DriverList::D3D11DriverList(IDXGIFactory1* dxgiFactory)
    {
        Enumerate(dxgiFactory);
    }

    D3D11DriverList::~D3D11DriverList(void)
    {
        for (size_t i = 0; i < _driverList.size(); i++)
        {
            te_delete(_driverList[i]);
        }

        _driverList.clear();
    }

    void D3D11DriverList::Enumerate(IDXGIFactory1* dxgiFactory)
    {
        UINT32 adapterIdx = 0;
        IDXGIAdapter* dxgiAdapter = nullptr;
        HRESULT hr;

        while ((hr = dxgiFactory->EnumAdapters(adapterIdx, &dxgiAdapter)) != DXGI_ERROR_NOT_FOUND)
        {
            if (FAILED(hr))
            {
                SAFE_RELEASE(dxgiAdapter);
                TE_ASSERT_ERROR(false, "Enumerating adapters failed.", __FILE__, __LINE__);
            }

            _driverList.push_back(te_new<D3D11Driver>(adapterIdx, dxgiAdapter));

            SAFE_RELEASE(dxgiAdapter);
            adapterIdx++;
        }
    }

    UINT32 D3D11DriverList::Count() const
    {
        return (UINT32)_driverList.size();
    }

    D3D11Driver* D3D11DriverList::Item(UINT32 idx) const
    {
        return _driverList.at(idx);
    }

    D3D11Driver* D3D11DriverList::Item(const String& name) const
    {
        for (auto it = _driverList.begin(); it != _driverList.end(); ++it)
        {
            if ((*it)->GetDriverDescription() == name)
                return (*it);
        }

        TE_ASSERT_ERROR(false, "Cannot find video mode with the specified name.", __FILE__, __LINE__);
        return nullptr;
    }
}