#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "Image/TeTextureManager.h"

namespace te
{
	/**	Handles creation of DirectX 11 textures. */
	class D3D11TextureManager : public TextureManager
	{
	public:
		/** @copydoc TextureManager::getNativeFormat */
		PixelFormat GetNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma) override;

	protected:		
		/** @copydoc TextureManager::createTextureInternal */
		SPtr<Texture> CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData = nullptr) override;
	};

    // TODO
}
