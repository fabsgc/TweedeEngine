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

        D3D11Driver& operator=(const D3D11Driver& r);

        /**	Returns the name of the driver. */
        String GetDriverName() const;

        /**	Returns the description of the driver. */
        String GetDriverDescription() const;

        /**	Returns adapter index of the adapter the driver is managing. */
        UINT32 GetAdapterNumber() const { return _adapterNumber; }

        /**	Returns number of outputs connected to the adapter the driver is managing. */
        UINT32 GetNumAdapterOutputs() const { return _numOutputs; }

        /**	Returns a description of the adapter the driver is managing. */
        const DXGI_ADAPTER_DESC& GetAdapterIdentifier() const { return _adapterIdentifier; }

        /**	Returns internal DXGI adapter object for the driver. */
        IDXGIAdapter* GetDeviceAdapter() const { return _DXGIAdapter; }

        /**	Returns description of an output device at the specified index. */
        DXGI_OUTPUT_DESC GetOutputDesc(UINT32 adapterOutputIdx) const;

        /**	Returns a list of all available video modes for all output devices. */
        SPtr<VideoModeInfo> GetVideoModeInfo() const { return _videoModeInfo; }

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