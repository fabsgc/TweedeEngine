#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Helper class that handles generation and processing of textures used for image based lighting. */
    class TE_CORE_EXPORT IBLUtility : public Module<IBLUtility>
    {
    public:

        TE_MODULE_STATIC_HEADER_MEMBER(IBLUtility)

        /**
            * Performs filtering on the cubemap, populating its mip-maps with filtered values that can be used for
            * evaluating specular reflections.
            *
            * @param[in, out]	cubemap		Cubemap to filter. Its mip level 0 will be read, filtered and written into
            *								other mip levels.
            * @param[in]		scratch		Temporary cubemap texture to use for the filtering process. Must match the size of
            *								the source cubemap. Provide null to automatically create a scratch cubemap.
            */
        virtual void FilterCubemapForSpecular(const SPtr<Texture>& cubemap, const SPtr<Texture>& scratch) const = 0;

        /**
         * Performs filtering on the cubemap, populating the output cubemap with values that can be used for evaluating
         * irradiance for use in diffuse lighting. Uses order-5 SH (25 coefficients) and outputs the values in the form of
         * a cubemap.
         *
         * @param[in]		cubemap		Cubemap to filter. Its mip level 0 will be used as source.
         * @param[in]		output		Output cubemap to store the irradiance data in.
         */
        virtual void FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output) const = 0;

        /**
         * Performs filtering on the cubemap, populating the output texture with values that can be used for evaluating
         * irradiance for use in diffuse lighting. Uses order-3 SH (9 coefficients) and outputs the values in the form of
         * SH coefficients.
         *
         * @param[in]		cubemap		Cubemap to filter. Its mip level 0 will be used as source.
         * @param[in]		output		Output texture in which to place the results. Must be allocated using
         *								IrradianceReduceMat::createOutputTexture();
         * @param[in]		outputIdx	Index in the output buffer at which to write the output coefficients to.
         */
        virtual void FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output,
            UINT32 outputIdx) const = 0;

        /**
         * Scales a cubemap and outputs it in the destination texture, using hardware acceleration. If both textures are the
         * same size, performs a copy instead.
         *
         * @param[in]   src				Source cubemap to scale.
         * @param[in]   srcMip			Determines which mip level of the source texture to scale.
         * @param[in]   dst				Desination texture to output the scaled data to. Must be usable as a render target.
         * @param[in]   dstMip			Determines which mip level of the destination texture to scale.
         */
        virtual void ScaleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst, UINT32 dstMip) const = 0;


        /** Returns the size of the texture required to store the provided number of SH coefficient sets. */
        static Vector2I GetSHCoeffTextureSize(UINT32 numCoeffSets, UINT32 shOrder);

        /**
         * Determines the position of a set of coefficients in the coefficient texture, depending on the coefficient index.
         */
        static Vector2I GetSHCoeffXYFromIdx(UINT32 idx, UINT32 shOrder);

    public:
        static const UINT32 REFLECTION_CUBEMAP_SIZE;
        static const UINT32 IRRADIANCE_CUBEMAP_SIZE;
    };

    /**	Provides easy access to IBLUtility. */
    TE_CORE_EXPORT const IBLUtility& gIBLUtility();
}
