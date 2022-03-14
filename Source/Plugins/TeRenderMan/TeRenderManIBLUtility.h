#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeIBLUtility.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeGpuResourcePool.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(ReflectionCubeDownsampleParamDef)
        TE_PARAM_BLOCK_ENTRY(int, gCubeFace)
        TE_PARAM_BLOCK_ENTRY(int, gMipLevel)
    TE_PARAM_BLOCK_END

        extern ReflectionCubeDownsampleParamDef gReflectionCubeDownsampleParamDef;

    /** Performs filtering on cubemap faces in order to prepare them for importance sampling. */
    class ReflectionCubeDownsampleMat : public RendererMaterial<ReflectionCubeDownsampleMat>
    {
        RMAT_DEF(BuiltinShader::ReflectionCubeDownsample)

    public:
        ReflectionCubeDownsampleMat();

        /** Downsamples the provided texture face and outputs it to the provided target. */
        void Execute(const SPtr<Texture>& source, UINT32 face, UINT32 mip, const SPtr<RenderTarget>& target);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
        SPtr<Texture> _texture;
    };

    /** Renderer implementation of IBLUtility. */
    class RenderManIBLUtility : public IBLUtility
    {
    public:
        /** @copydoc IBLUtility::filterCubemapForSpecular */
        void FilterCubemapForSpecular(const SPtr<Texture>& cubemap, const SPtr<Texture>& scratch) const override;

        /** @copydoc IBLUtility::filterCubemapForIrradiance(const SPtr<Texture>&, const SPtr<Texture>&) const */
        void FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output) const override;

        /** @copydoc IBLUtility::filterCubemapForIrradiance(const SPtr<Texture>&, const SPtr<Texture>&, UINT32) const */
        void FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output,
            UINT32 outputIdx) const override;

        /** @copydoc IBLUtility::scaleCubemap */
        void ScaleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst, UINT32 dstMip) const override;

    private:
        /**
         * Downsamples a cubemap using hardware bilinear filtering.
         *
         * @param[in]	src		Cubemap to downsample.
         * @param[in]   srcMip	Determines which mip level of the source texture to downsample.
         * @param[in]   dst		Desination texture to output the scaled data to. Must be usable as a render target.
         * @param[in]   dstMip	Determines which mip level of the destination texture to scale.
         */
        static void DownsampleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst, UINT32 dstMip);

        /**
         * Generates irradiance SH coefficients from the input cubemap and writes them to a 1D texture. Does not make
         * use of the compute shader.
         */
        static void FilterCubemapForIrradianceNonCompute(const SPtr<Texture>& cubemap, UINT32 outputIdx,
            const SPtr<RenderTexture>& output);
    };
}
