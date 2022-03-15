#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeIBLUtility.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeGpuResourcePool.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(ReflectionCubeDownsampleParamDef)
        TE_PARAM_BLOCK_ENTRY(UINT32, gCubeFace)
        TE_PARAM_BLOCK_ENTRY(UINT32, gMipLevel)
        TE_PARAM_BLOCK_ENTRY(Vector2, gPadding)
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
    };

    TE_PARAM_BLOCK_BEGIN(ReflectionCubeImportanceSampleParamDef)
        TE_PARAM_BLOCK_ENTRY(UINT32, gCubeFace)
        TE_PARAM_BLOCK_ENTRY(UINT32, gMipLevel)
        TE_PARAM_BLOCK_ENTRY(UINT32, gNumMips)
        TE_PARAM_BLOCK_ENTRY(UINT32, gNumSamples)
        TE_PARAM_BLOCK_ENTRY(float, gPrecomputedMipFactor)
        TE_PARAM_BLOCK_ENTRY(Vector3, gPadding)
    TE_PARAM_BLOCK_END

    extern ReflectionCubeImportanceSampleParamDef gReflectionCubeImportanceSampleParamDef;

    /** Performs importance sampling on cubemap faces in order for make them suitable for specular evaluation. */
    class ReflectionCubeImportanceSampleMat : public RendererMaterial<ReflectionCubeImportanceSampleMat>
    {
        RMAT_DEF(BuiltinShader::ReflectionCubeImportanceSample)

    public:
        ReflectionCubeImportanceSampleMat();

        /** Importance samples the provided texture face and outputs it to the provided target. */
        void Execute(const SPtr<Texture>& source, UINT32 face, UINT32 mip, const SPtr<RenderTarget>& target);

    private:
        static const UINT32 NUM_SAMPLES;

        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };

    /** Vector representing spherical harmonic coefficients for 5 bands. */
    struct SHVector5
    {
        SHVector5()
            : Coeffs()
        { }

        float Coeffs[25];
    };

    /** Vector representing spherical coefficients for 5 bands, separate for red, green and blue components. */
    struct SHVector5RGB
    {
        SHVector5 R, G, B;
    };

    /** Intermediate structure used for spherical coefficient calculation. Contains RGB coefficients and weight. */
    struct SHCoeffsAndWeight5
    {
        SHVector5RGB Coeffs;
        float Weight;
    };

    TE_PARAM_BLOCK_BEGIN(IrradianceComputeSHParamDef)
        TE_PARAM_BLOCK_ENTRY(UINT32, gCubeFace)
        TE_PARAM_BLOCK_ENTRY(UINT32, gFaceSize)
        TE_PARAM_BLOCK_ENTRY(Vector2I, gDispatchSize)
    TE_PARAM_BLOCK_END

    extern IrradianceComputeSHParamDef gIrradianceComputeSHParamDef;

    /** Computes spherical harmonic coefficients from a radiance cubemap. */
    class IrradianceComputeSHMat : public RendererMaterial<IrradianceComputeSHMat>
    {
        RMAT_DEF(BuiltinShader::IrradianceComputeSH)

    public:
        IrradianceComputeSHMat();

        /**
         * Computes spherical harmonic coefficients from a radiance texture and outputs a buffer containing a list of
         * coefficient sets (one set of coefficients for each thread group). Coefficients must be reduced and normalized
         * by IrradianceReduceSHMat before use. Output buffer should be created by calling createOutputBuffer().
         */
        void Execute(const SPtr<Texture>& source, UINT32 face, const SPtr<GpuBuffer>& output);

        /** Creates a buffer of adequate size to be used as output for this material. */
        SPtr<GpuBuffer> CreateOutputBuffer(const SPtr<Texture>& source, UINT32& numCoeffSets);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };

    TE_PARAM_BLOCK_BEGIN(IrradianceReduceSHParamDef)
        TE_PARAM_BLOCK_ENTRY(Vector2I, gOutputIdx)
        TE_PARAM_BLOCK_ENTRY(UINT32, gNumEntries)
        TE_PARAM_BLOCK_ENTRY(UINT32, gpadding)
    TE_PARAM_BLOCK_END

    extern IrradianceReduceSHParamDef gIrradianceReduceSHParamDef;

    /**
     * Sums spherical harmonic coefficients calculated by each thread group of IrradianceComputeSHMat and outputs a single
     * set of normalized coefficients.
     */
    class IrradianceReduceSHMat : public RendererMaterial<IrradianceReduceSHMat>
    {
        RMAT_DEF(BuiltinShader::IrradianceReduceSH)

    public:
        IrradianceReduceSHMat();

        /**
         * Sums spherical harmonic coefficients calculated by each thread group of IrradianceComputeSHMat and outputs a
         * single set of normalized coefficients. Output texture should be created by calling createOutputTexture(). The
         * value will be recorded at the @p outputIdx position in the texture.
         */
        void Execute(const SPtr<GpuBuffer>& source, UINT32 numCoeffSets, const SPtr<Texture>& output, UINT32 outputIdx);

        /** Creates a texture of adequate size to be used as output for this material. */
        SPtr<Texture> CreateOutputTexture(UINT32 numCoeffSets);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };

    TE_PARAM_BLOCK_BEGIN(IrradianceProjectSHParamDef)
        TE_PARAM_BLOCK_ENTRY(UINT32, gCubeFace)
        TE_PARAM_BLOCK_ENTRY(Vector3, gPadding)
    TE_PARAM_BLOCK_END

    extern IrradianceProjectSHParamDef gIrradianceProjectSHParamDef;

    /**
     * Projects spherical harmonic coefficients calculated by IrradianceReduceSHMat and projects them onto faces of
     * a cubemap.
     */
    class IrradianceProjectSHMat : public RendererMaterial<IrradianceProjectSHMat>
    {
        RMAT_DEF(BuiltinShader::IrradianceProjectSH)

    public:
        IrradianceProjectSHMat();

        /**
         * Projects spherical harmonic coefficients calculated by IrradianceReduceSHMat and projects them onto faces of
         * a cubemap.
         */
        void Execute(const SPtr<Texture>& shCoeffs, UINT32 face, const SPtr<RenderTarget>& target);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
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
    };
}
