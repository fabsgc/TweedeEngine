#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeRenderQueue.h"

namespace te
{
    /** A set of options used for controlling the rendering of the RenderMan renderer. */
    struct RenderManOptions : public RendererOptions
    {
        /**
         * Determines the maximum shadow map size, in pixels. The system might decide to use smaller resolution maps for
         * shadows far away, but will never increase the resolution past the provided value.
         */
        UINT32 ShadowMapSize = 2048;

        /**
         * Determines which occlusion are currently used to cull objects before rendering. Note that frustum culling can be
         * CPU time consuming if scene partitioning does not use an efficient algorithm
         */
        UINT32 CullingFlags = (UINT32)RenderManCulling::Frustum | (UINT32)RenderManCulling::Occlusion;

        /**
         * Controls if and how a render queue groups renderable objects by material in order to reduce number of state
         * changes. Sorting by material can reduce CPU usage but could increase overdraw.
         */
        StateReduction ReductionMode = StateReduction::Distance;

        /**
         * By default, we will try to batch objects which share same geometry and same material
        */
        RenderManInstancing InstancingMode = RenderManInstancing::Manual;
    };
}
