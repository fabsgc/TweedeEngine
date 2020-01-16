#include "TeD3D11RenderTexture.h"
#include "TeD3D11TextureView.h"

namespace te
{
    D3D11RenderTexture::D3D11RenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
		: RenderTexture(desc, deviceIdx)
        , _properties(desc, false)
	{
		assert(deviceIdx == 0 && "Multiple GPUs not supported natively on DirectX 11.");
	}

	void D3D11RenderTexture::GetCustomAttribute(const String& name, void* data) const
	{
		if(name == "RTV")
		{
			ID3D11RenderTargetView** rtvs = (ID3D11RenderTargetView**)data;
			for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; ++i)
			{
				if (_colorSurfaces[i] == nullptr)
				{
				    return;
                }

				D3D11TextureView* textureView = static_cast<D3D11TextureView*>(_colorSurfaces[i].get());
				rtvs[i] = textureView->GetRTV();
			}
		}
		else if(name == "DSV")
		{
			if (_depthStencilSurface == nullptr)
			{
				return;
            }

			ID3D11DepthStencilView** dsv = (ID3D11DepthStencilView**)data;
			D3D11TextureView* depthStencilView = static_cast<D3D11TextureView*>(_depthStencilSurface.get());

			*dsv = depthStencilView->GetDSV(false, false);
		}
		else if (name == "RODSV")
		{
			if (_depthStencilSurface == nullptr)
			{
				return;
            }

			ID3D11DepthStencilView** dsv = (ID3D11DepthStencilView**)data;
			D3D11TextureView* depthStencilView = static_cast<D3D11TextureView*>(_depthStencilSurface.get());

			*dsv = depthStencilView->GetDSV(true, true);
		}
		else if (name == "RODWSV")
		{
			if (_depthStencilSurface == nullptr)
			{
				return;
            }

			ID3D11DepthStencilView** dsv = (ID3D11DepthStencilView**)data;
			D3D11TextureView* depthStencilView = static_cast<D3D11TextureView*>(_depthStencilSurface.get());

			*dsv = depthStencilView->GetDSV(true, false);
		}
		else if (name == "WDROSV")
		{
			if (_depthStencilSurface == nullptr)
            {
				return;
            }

			ID3D11DepthStencilView** dsv = (ID3D11DepthStencilView**)data;
			D3D11TextureView* depthStencilView = static_cast<D3D11TextureView*>(_depthStencilSurface.get());

			*dsv = depthStencilView->GetDSV(false, true);
		}
	}
}
