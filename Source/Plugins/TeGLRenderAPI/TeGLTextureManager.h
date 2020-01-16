#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "Image/TeTextureManager.h"

namespace te
{
	/**	Handles creation of OpenGL textures. */
	class GLTextureManager : public TextureManager
	{
	public:
		/** @copydoc TextureManager::GetNativeFormat */
		PixelFormat GetNativeFormat(TextureType type, PixelFormat format, int usage, bool hwGamma) override;

	protected:		
		/** @copydoc TextureManager::CreateTextureInternal */
		SPtr<Texture> CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData = nullptr) override;

        /** @copydoc TextureManager::CreateRenderTextureInternal */
        SPtr<RenderTexture> CreateRenderTextureInternal(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx = 0) override;
	};

    // TODO
}
