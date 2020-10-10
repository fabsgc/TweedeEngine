#include "TeD3D11VideoModeInfo.h"

namespace te
{
    D3D11VideoModeInfo::D3D11VideoModeInfo(IDXGIAdapter* dxgiAdapter)
    {
        UINT32 outputIdx = 0;
        IDXGIOutput* output = nullptr;
        while (dxgiAdapter->EnumOutputs(outputIdx, &output) != DXGI_ERROR_NOT_FOUND)
        {
            _outputs.push_back(te_new<D3D11VideoOutputInfo>(output, outputIdx));
            outputIdx++;
        }
    }

    D3D11VideoOutputInfo::D3D11VideoOutputInfo(IDXGIOutput* output, UINT32 outputIdx)
        : _DXGIOutput(output)
    {
        DXGI_OUTPUT_DESC outputDesc;
        output->GetDesc(&outputDesc);
        _name = ToString(WString(outputDesc.DeviceName));

        UINT32 numModes = 0;

        HRESULT hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, nullptr);
        if (FAILED(hr))
        {
            SAFE_RELEASE(output);
            TE_ASSERT_ERROR(false, "Error while enumerating adapter output video modes.");
        }

        DXGI_MODE_DESC* modeDesc = te_newN<DXGI_MODE_DESC>(numModes);

        hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modeDesc);
        if (FAILED(hr))
        {
            te_deleteN(modeDesc, numModes);

            SAFE_RELEASE(output);
            TE_ASSERT_ERROR(false, "Error while enumerating adapter output video modes.");
        }

        for (UINT32 i = 0; i < numModes; i++)
        {
            DXGI_MODE_DESC displayMode = modeDesc[i];

            bool foundVideoMode = false;
            for (auto videoMode : _videoModes)
            {
                D3D11VideoMode* d3d11videoMode = static_cast<D3D11VideoMode*>(videoMode);

                if (d3d11videoMode->_width == displayMode.Width && d3d11videoMode->_height == displayMode.Height &&
                    d3d11videoMode->_refreshRateNumerator == displayMode.RefreshRate.Numerator &&
                    d3d11videoMode->_refreshRateDenominator == displayMode.RefreshRate.Denominator)
                {
                    foundVideoMode = true;
                    break;
                }
            }

            if (!foundVideoMode)
            {
                float refreshRate = displayMode.RefreshRate.Numerator / (float)displayMode.RefreshRate.Denominator;
                D3D11VideoMode* videoMode = te_new<D3D11VideoMode>(displayMode.Width, displayMode.Height, refreshRate,
                    outputIdx, displayMode.RefreshRate.Numerator, displayMode.RefreshRate.Denominator, displayMode);

                _videoModes.push_back(videoMode);
            }
        }

        te_deleteN(modeDesc, numModes);

        // Get desktop display mode
        HMONITOR hMonitor = outputDesc.Monitor;
        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);

        DEVMODE devMode;
        devMode.dmSize = sizeof(DEVMODE);
        devMode.dmDriverExtra = 0;
        EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

        DXGI_MODE_DESC currentMode;
        currentMode.Width = devMode.dmPelsWidth;
        currentMode.Height = devMode.dmPelsHeight;
        bool useDefaultRefreshRate = 1 == devMode.dmDisplayFrequency || 0 == devMode.dmDisplayFrequency;
        currentMode.RefreshRate.Numerator = useDefaultRefreshRate ? 0 : devMode.dmDisplayFrequency;
        currentMode.RefreshRate.Denominator = useDefaultRefreshRate ? 0 : 1;
        currentMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        currentMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        currentMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        DXGI_MODE_DESC nearestMode;
        ZeroMemory(&nearestMode, sizeof(nearestMode));

        output->FindClosestMatchingMode(&currentMode, &nearestMode, nullptr);

        float refreshRate = nearestMode.RefreshRate.Numerator / (float)nearestMode.RefreshRate.Denominator;
        _desktopVideoMode = te_new<D3D11VideoMode>(nearestMode.Width, nearestMode.Height, refreshRate,
            outputIdx, nearestMode.RefreshRate.Numerator, nearestMode.RefreshRate.Denominator, nearestMode);
    }

    D3D11VideoOutputInfo::~D3D11VideoOutputInfo()
    {
        SAFE_RELEASE(_DXGIOutput);
    }

    D3D11VideoMode::D3D11VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx,
        UINT32 refreshRateNumerator, UINT32 refreshRateDenominator, DXGI_MODE_DESC mode)
        : VideoMode(width, height, refreshRate, outputIdx), _refreshRateNumerator(refreshRateNumerator),
        _refreshRateDenominator(refreshRateDenominator), _D3D11Mode(mode)
    { }
}