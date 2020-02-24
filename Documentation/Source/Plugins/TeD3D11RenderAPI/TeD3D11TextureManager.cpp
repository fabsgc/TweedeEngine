#include "TeD3D11TextureManager.h"
#include "TeD3D11Texture.h"
#include "TeD3D11Mappings.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11RenderTexture.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(D3D11TextureManager)

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

    SPtr<RenderTexture> D3D11TextureManager::CreateRenderTextureInternal(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
    {
        SPtr<D3D11RenderTexture> texPtr = te_core_ptr<D3D11RenderTexture>(new (te_allocate<D3D11RenderTexture>()) D3D11RenderTexture(desc, deviceIdx));
        texPtr->SetThisPtr(texPtr);

        return texPtr;
    }
}
