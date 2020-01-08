#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11DriverList.h"
#include "TeD3D11Driver.h"
#include "TeD3D11InputLayoutManager.h"
#include "TeD3D11HLSLProgramFactory.h"
#include "Math/TeRect2.h"

namespace te
{
    class D3D11RenderAPI: public RenderAPI
    {
    public:
        D3D11RenderAPI();
        ~D3D11RenderAPI();

        SPtr<RenderWindow> CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc) override;
        void Initialize() override;
        void Update() override;

        /**	Returns the main DXGI factory object. */
		IDXGIFactory1* getDXGIFactory() const { return _DXGIFactory; }

		/**	Returns the primary DX11 device object. */
		D3D11Device& getPrimaryDevice() const { return *_device; }

        /**	Returns information describing all available drivers. */
		D3D11DriverList* GetDriverList() const { return _driverList; }

    private:
        IDXGIFactory1* _DXGIFactory = nullptr;
		D3D11Device* _device = nullptr;

        D3D11DriverList* _driverList = nullptr;
		D3D11Driver* _activeD3DDriver = nullptr;

        D3D_FEATURE_LEVEL _featureLevel = ::D3D_FEATURE_LEVEL_11_0;

        D3D11HLSLProgramFactory* _HLSLFactory = nullptr;
        D3D11InputLayoutManager* _IAManager = nullptr;

        UINT32 _stencilRef = 0;
		Rect2 _viewportNorm = Rect2(0.0f, 0.0f, 1.0f, 1.0f);
        D3D11_VIEWPORT _viewport;
        D3D11_RECT _scissorRect;
    };
}