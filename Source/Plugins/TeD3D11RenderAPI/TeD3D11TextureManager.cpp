#include "TeD3D11TextureManager.h"
#include "TeD3D11Texture.h"
#include "TeD3D11Mappings.h"
#include "TeD3D11RenderAPI.h"

namespace te
{
	PixelFormat D3D11TextureManager::GetNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
	{
		DXGI_FORMAT d3dPF = D3D11Mappings::GetPF(D3D11Mappings::GetClosestSupportedPF(format, ttype, usage), hwGamma);
		return D3D11Mappings::GetPF(d3dPF);
	}

	SPtr<Texture> D3D11TextureManager::CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData)
	{
        SPtr<D3D11Texture> texPtr = te_core_ptr<D3D11Texture>(new (te_allocate<D3D11Texture>()) D3D11Texture(desc, initialData));
		texPtr->SetThisPtr(texPtr);

		return texPtr;
	}
}
