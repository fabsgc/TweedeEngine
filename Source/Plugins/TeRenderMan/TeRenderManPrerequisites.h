#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
    /** Available implementation of the RenderElement class. */
	enum class RenderElementType
	{
		/** See RenderableElement. */
		Renderable,
		/** See ParticlesRenderElement. */
		Particle,
		/** See DecalRenderElement. */
		Decal
	};

	/** Types of ways for shaders to handle MSAA. */
	enum class MSAAMode
	{
		/** No MSAA supported. */
		None,
		/** Single MSAA sample will be resolved. */
		Single,
		/** All MSAA samples will be resolved. */
		Full,
	};

    /** Information about current time and frame index. */
	struct FrameTimings
	{
		float time = 0.0f;
		float timeDelta = 0.0f;
		UINT64 frameIdx = 0;
	};

    struct RenderManOptions;
    class RendererScene;
}
