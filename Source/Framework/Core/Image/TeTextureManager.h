#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "TePixelData.h"
#include "TeTexture.h"

namespace te
{
    /** Defines interface for creation of textures. Render systems provide their own implementations. */
    class TE_CORE_EXPORT TextureManager : public Module<TextureManager>
    {
    public:
        virtual ~TextureManager() = default;

        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /** @copydoc Texture::create(const TEXTURE_DESC&) */
        SPtr<Texture> CreateTexture(const TEXTURE_DESC& desc);

        /**
         * Creates a new 2D or 3D texture initialized using the provided pixel data. Texture will not have any mipmaps.
         *
         * @param[in]	desc  		Description of the texture to create. Must match the pixel data.
         * @param[in]	pixelData	Data to initialize the texture width.
         */
        SPtr<Texture> CreateTexture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData);

        /**
         * Creates an empty and uninitialized texture of a specific type. This is to be implemented	by render systems with
         * their own implementations.
         */
        virtual SPtr<Texture> CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData = nullptr) = 0;

        /** Gets the format which will be natively used for a requested format given the constraints of the current device. */
        virtual PixelFormat GetNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma) = 0;

    protected:
        friend class Texture;
    };
}
