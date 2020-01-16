#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "Image/TeTextureManager.h"

namespace te
{
	/**	Handles creation of DirectX 11 textures. */
	class D3D11TextureManager : public TextureManager
	{
	public:
		/** @copydoc TextureManager::GetNativeFormat */
		PixelFormat GetNativeFormat(TextureType type, PixelFormat format, int usage, bool hwGamma) override;

	protected:		
		/** @copydoc TextureManager::createTextureInternal */
		SPtr<Texture> CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData = nullptr) override;

        /** @copydoc TextureManager::CreateRenderTextureInternal */
        SPtr<RenderTexture> CreateRenderTextureInternal(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx = 0) override;
	};
}
