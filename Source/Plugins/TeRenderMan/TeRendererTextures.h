#pragma once

#include "TeRenderManPrerequisites.h"

namespace te
{
    /** Contains static textures required for various render techniques. */
    class RendererTextures
    {
    public:
        /** Initializes the renderer textures. Must be called before using the textures. */
        static void StartUp();

        /** Cleans up renderer textures. */
        static void ShutDown();

        /** Two modes are avaibles to compute PreIntegratedEnvGF : GXX and Charlie */
        enum class DistributionMode
        {
            GXX, Charlie
        };

        /**
         * 2D 2-channel texture containing a pre-integrated G and F factors of the microfactet BRDF. This is an
         * approximation used for image based lighting, so we can avoid sampling environment maps for each light. Works in
         * tandem with the importance sampled reflection cubemaps.
         *
         * (u, v) = (NoV, roughness)
         * (r, g) = (scale, bias)
         */
        static SPtr<Texture> PreIntegratedEnvGF_GXX;

        /**
         * 2D 2-channel texture containing a pre-integrated G and F factors of the microfactet BRDF. This is an
         * approximation used for image based lighting, so we can avoid sampling environment maps for each light. Works in
         * tandem with the importance sampled reflection cubemaps.
         *
         * (u, v) = (NoV, roughness)
         * (r, g) = (scale, bias)
         */
        static SPtr<Texture> PreIntegratedEnvGF_Charlie;
    };
}
