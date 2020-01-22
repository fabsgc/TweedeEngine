#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"

namespace te
{
	/** Texture filtering options for RenderMan. */
	enum class RenderManFiltering
	{
		Bilinear, /**< Sample linearly in X and Y directions within a texture mip level. */
		Trilinear, /**< Sample bilinearly and also between texture mip levels to hide the mip transitions. */
		Anisotropic /**< High quality dynamic filtering that improves quality of angled surfaces */
	};

	/** A set of options used for controlling the rendering of the RenderMan renderer. */
	struct RenderManOptions : public RendererOptions
	{
		/**	Type of filtering to use for all textures on scene elements. */
		RenderManFiltering Filtering = RenderManFiltering::Anisotropic;

		/**
		 * Maximum number of samples to be used when performing anisotropic filtering. Only relevant if #filtering is set to
		 * RenderManFiltering::Anisotropic.
		 */
		UINT32 AnisotropyMax = 16;

		/**
		 * Determines the maximum shadow map size, in pixels. The system might decide to use smaller resolution maps for
		 * shadows far away, but will never increase the resolution past the provided value.
		 */
		UINT32 ShadowMapSize = 2048;
	};
}
