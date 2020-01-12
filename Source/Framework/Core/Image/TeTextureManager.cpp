#include "TeTextureManager.h"

namespace te
{
    SPtr<Texture> TextureManager::CreateTexture(const TEXTURE_DESC& desc)
    {
        SPtr<Texture> newTex = CreateTextureInternal(desc, nullptr);
        newTex->Initialize();

        return newTex;
    }

    SPtr<Texture> TextureManager::CreateTexture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData)
    {
        SPtr<Texture> newTex = CreateTextureInternal(desc, pixelData);
        newTex->Initialize();

        return newTex;
    }
}
