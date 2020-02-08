#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeRenderQueue.h"

namespace te
{
    /** A set of options used for controlling the rendering of the RenderMan renderer. */
    struct RenderManOptions : public RendererOptions
    {
        /** Type of filtering to use for all textures on scene elements. */
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

        /**
         * Controls if and how a render queue groups renderable objects by material in order to reduce number of state
         * changes. Sorting by material can reduce CPU usage but could increase overdraw.
         */
        StateReduction ReductionMode = StateReduction::Distance;

        /**
         * By default, we will try to batch object who share same geometry and same material
        */
        RenderManInstancing InstancingMode = RenderManInstancing::Manual;
    };
}
