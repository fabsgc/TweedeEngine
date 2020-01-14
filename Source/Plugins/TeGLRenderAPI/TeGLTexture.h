//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "Image/TeTexture.h"

namespace te
{
	/**	OpenGL implementation of a texture. */
    class GLTexture : public Texture
    {
    public:
        ~GLTexture();

    protected:
        friend class GLTextureManager;

        GLTexture(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData);

        /** @copydoc CoreObject::Initialize() */
        void Initialize() override;

        /** @copydoc Texture::LockImpl */
		PixelData LockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

		/** @copydoc Texture::UnlockImpl */
		void UnlockImpl() override;

		/** @copydoc Texture::CopyImpl */
		void CopyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc) override;

		/** @copydoc Texture::ReadData */
		void ReadDataImpl(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

		/** @copydoc Texture::WriteData */
		void WriteDataImpl(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false, UINT32 queueIdx = 0) override;
    };
}
