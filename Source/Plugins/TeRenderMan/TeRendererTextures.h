#pragma once

#include "TeRenderManPrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Contains static textures required for various render techniques. */
    class RendererTextures : public Module<RendererTextures>
    {
    public:
        /** Initializes the renderer textures. Must be called before using the textures. */
        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** Cleans up renderer textures. */
        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /** Two modes are avaibles to compute PreIntegratedEnvGF : GGX and Charlie */
        enum class DistributionMode
        {
            GGX, Charlie
        };

        /**
         * 2D 2-channel texture containing a pre-integrated G and F factors of the microfactet BRDF. This is an
         * approximation used for image based lighting, so we can avoid sampling environment maps for each light. Works in
         * tandem with the importance sampled reflection cubemaps.
         *
         * (u, v) = (NoV, roughness)
         * (r, g, b) = (scale, bias, charlie)
         */
        SPtr<Texture> PreIntegratedEnvGF;

        /** Tileable 4x4 texture to be used for randomization in SSAO rendering. */
        SPtr<Texture> SSAORandomization4x4;
    };

    RendererTextures& gRendererTextures();
}
