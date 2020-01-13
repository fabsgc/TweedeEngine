#include "TeTextureManager.h"
#include "Utility/TeColor.h"

namespace te
{
    void TextureManager::OnStartUp()
    {
        TEXTURE_DESC desc;
        desc.Type = TEX_TYPE_2D;
        desc.Width = 2;
        desc.Height = 2;
        desc.Format = PF_RGBA8;
        desc.Usage = TU_STATIC;

        // White built-in texture
        SPtr<Texture> whiteTexture = CreateTexture(desc);

        SPtr<PixelData> whitePixelData = PixelData::Create(2, 2, 1, PF_RGBA8);
        whitePixelData->SetColorAt(Color::White, 0, 0);
        whitePixelData->SetColorAt(Color::White, 0, 1);
        whitePixelData->SetColorAt(Color::White, 1, 0);
        whitePixelData->SetColorAt(Color::White, 1, 1);

        whiteTexture->WriteData(*whitePixelData);
        Texture::WHITE = whiteTexture;

        // Black built-in texture
        SPtr<Texture> blackTexture = CreateTexture(desc);

        SPtr<PixelData> blackPixelData = PixelData::Create(2, 2, 1, PF_RGBA8);
        blackPixelData->SetColorAt(Color::ZERO, 0, 0);
        blackPixelData->SetColorAt(Color::ZERO, 0, 1);
        blackPixelData->SetColorAt(Color::ZERO, 1, 0);
        blackPixelData->SetColorAt(Color::ZERO, 1, 1);

        blackTexture->WriteData(*blackPixelData);
        Texture::BLACK = blackTexture;

        // Normal (Y = Up) built-in texture
        SPtr<Texture> normalTexture = CreateTexture(desc);
        SPtr<PixelData> normalPixelData = PixelData::Create(2, 2, 1, PF_RGBA8);

        Color encodedNormal(0.5f, 0.5f, 1.0f);
        normalPixelData->SetColorAt(encodedNormal, 0, 0);
        normalPixelData->SetColorAt(encodedNormal, 0, 1);
        normalPixelData->SetColorAt(encodedNormal, 1, 0);
        normalPixelData->SetColorAt(encodedNormal, 1, 1);

        normalTexture->WriteData(*normalPixelData);
        Texture::NORMAL = normalTexture;
    }

    void TextureManager::OnShutDown()
    {
        // Need to make sure these are freed while still on the core thread
        Texture::WHITE = nullptr;
        Texture::BLACK = nullptr;
        Texture::NORMAL = nullptr;
    }

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
