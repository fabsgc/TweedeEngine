#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeVideoMode.h"

namespace te
{
    class D3D11VideoMode : public VideoMode
    {
    public:
        D3D11VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx, UINT32 refreshRateNumerator,
            UINT32 refreshRateDenominator, DXGI_MODE_DESC mode);

        /** Returns an internal DXGI representation of this video mode. */
        const DXGI_MODE_DESC& GetDXGIModeDesc() const { return _D3D11Mode; }

        /** Gets internal DX11 refresh rate numerator. */
        UINT32 GetRefreshRateNumerator() const { return _refreshRateNumerator; }

        /** Gets internal DX11 refresh rate denominator. */
        UINT32 GetRefreshRateDenominator() const { return _refreshRateDenominator; }

    private:
        friend class D3D11VideoOutputInfo;

        UINT32 _refreshRateNumerator;
        UINT32 _refreshRateDenominator;
        DXGI_MODE_DESC _D3D11Mode;
    };

    class D3D11VideoOutputInfo : public VideoOutputInfo
    {
    public:
        D3D11VideoOutputInfo(IDXGIOutput* output, UINT32 outputIdx);
        virtual ~D3D11VideoOutputInfo();

        /** Returns the internal DXGI object representing an output device. */
        IDXGIOutput* GetDXGIOutput() const { return _DXGIOutput;  }

    private:
        IDXGIOutput* _DXGIOutput;
    };

    class D3D11VideoModeInfo : public VideoModeInfo
    {
    public:
        D3D11VideoModeInfo(IDXGIAdapter* dxgiAdapter);
    };
}
