#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"

namespace te
{
    class D3D11DriverList
    {
    public:
        /**	Constructs a new driver list from an existing DXGI factory object. */
        D3D11DriverList(IDXGIFactory1* dxgiFactory);
        ~D3D11DriverList();

        /**	Returns the number of available drivers. */
        UINT32 Count() const;

        /**	Returns a driver at the specified index. */
        D3D11Driver* Item(UINT32 idx) const;

        /**	Returns a driver with the specified name, or null if it cannot be found. */
        D3D11Driver* Item(const String& name) const;

    private:
        /**	Enumerates the DXGI factory object and constructs a list of available drivers. */
        void Enumerate(IDXGIFactory1* dxgiFactory);

        Vector<D3D11Driver*> _driverList;
    };
}