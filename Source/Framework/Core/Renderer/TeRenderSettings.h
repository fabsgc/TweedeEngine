#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"
#include "Image/TeColor.h"
#include "Image/TeTexture.h"

namespace te
{
    /** Base class for both sim and core thread variants of RenderSettings. */
    struct TE_CORE_EXPORT RenderSettings
    {
        RenderSettings() = default;
        ~RenderSettings() = default;

        /**
         * Determines if scene objects will be lit by lights. If disabled everything will be rendered using their albedo
         * texture with no lighting applied.
         */
        bool EnableLighting = true;

        /** Determines if shadows cast by lights should be rendered. Only relevant if lighting is turned on. */
        bool EnableShadows = true;

        /**
         * Signals the renderer to only render overlays (like GUI), and not scene objects. Such rendering doesn't require
         * depth buffer or multi-sampled render targets and will not render any scene objects. This can improve performance
         * and memory usage for overlay-only views.
         */
        bool OverlayOnly = false;

        /**
         * If enabled the camera will use the skybox for rendering the background. A skybox has to be present in the scene.
         * When disabled the camera will use the clear color for rendering the background.
         */
        bool EnableSkybox = true;

        /**
         * The absolute base cull-distance for objects rendered through this camera in world units. Objects will use this
         * distance and apply their own factor to it to determine whether they should be visible.
         */
        float CullDistance = FLT_MAX;
    };
}
