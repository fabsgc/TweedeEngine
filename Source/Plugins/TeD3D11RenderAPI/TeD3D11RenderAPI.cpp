#include "TeD3D11RenderAPI.h"
#include "TeD3D11RenderWindow.h"

namespace te
{
    D3D11RenderAPI::D3D11RenderAPI()
    {
    }

    D3D11RenderAPI::~D3D11RenderAPI()
    {
    }

    SPtr<RenderWindow> D3D11RenderAPI::CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc)
    {
        return te_shared_ptr_new<D3D11RenderWindow>(windowDesc);
    }

    void D3D11RenderAPI::Initialize()
    {
    }

    void D3D11RenderAPI::Update()
    {
    }
}