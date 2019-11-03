#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeVideoMode.h"

namespace te
{
    class D3D11Driver
    {
    public:
        /** Constructs a new object from the adapter number provided by DX11 runtime, and DXGI adapter object. */
        D3D11Driver(UINT32 adapterNumber, IDXGIAdapter* dxgiAdapter);
        D3D11Driver(const D3D11Driver& ob);
        ~D3D11Driver();

        /**	Returns the description of the driver. */
        String GetDriverDescription() const { return "empty"; }

    private:
        /**	Initializes the internal data. */
        void Construct();

    private:
        UINT32 _adapterNumber;
        UINT32 _numOutputs;
        DXGI_ADAPTER_DESC _adapterIdentifier;
        IDXGIAdapter* _DXGIAdapter;
        SPtr<VideoModeInfo> _videoModeInfo;
    };
}