#include "TeRenderManIBLUtility.h"
#include "Image/TeTexture.h"
#include "Image/TePixelUtil.h"
#include "RenderAPI/TeGpuBuffer.h"
#include "Renderer/TeRendererUtility.h"
#include "Renderer/TeTextureCubeDownsampleMat.h"

namespace te
{
    const UINT32 ReflectionCubeImportanceSampleMat::NUM_SAMPLES = 1024;
    ReflectionCubeImportanceSampleParamDef gReflectionCubeImportanceSampleParamDef;

    ReflectionCubeImportanceSampleMat::ReflectionCubeImportanceSampleMat()
    {
        _paramBuffer = gReflectionCubeImportanceSampleParamDef.CreateBuffer();
    }

    void ReflectionCubeImportanceSampleMat::Initialize()
    {
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void ReflectionCubeImportanceSampleMat::Execute(const SPtr<Texture>& source, UINT32 face, UINT32 mip,
        const SPtr<RenderTarget>& target)
    {
        gReflectionCubeImportanceSampleParamDef.gCubeFace.Set(_paramBuffer, face);
        gReflectionCubeImportanceSampleParamDef.gMipLevel.Set(_paramBuffer, mip);
        gReflectionCubeImportanceSampleParamDef.gNumMips.Set(_paramBuffer, source->GetProperties().GetNumMipmaps() + 1);
        gReflectionCubeImportanceSampleParamDef.gNumSamples.Set(_paramBuffer, ReflectionCubeImportanceSampleMat::NUM_SAMPLES);

        _params->SetTexture("SourceMap", source);

        float width = (float)source->GetProperties().GetWidth();
        float height = (float)source->GetProperties().GetHeight();

        // First part of the equation for determining mip level to sample from.
        // See http://http.developer.nvidia.com/GPUGems3/gpugems3_ch20.html
        float mipFactor = 0.5f * std::log2(width * height / NUM_SAMPLES);
        gReflectionCubeImportanceSampleParamDef.gPrecomputedMipFactor.Set(_paramBuffer, mipFactor);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(target);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }

    IrradianceComputeSHParamDef gIrradianceComputeSHParamDef;

    // TILE_WIDTH * TILE_HEIGHT must be pow2 because of parallel reduction algorithm
    const static UINT32 TILE_WIDTH = 8;
    const static UINT32 TILE_HEIGHT = 8;

    // For very small textures this should be reduced so number of launched threads can properly utilize GPU cores
    const static UINT32 PIXELS_PER_THREAD = 4;

    const static UINT32 SH_ORDER = 5;

    IrradianceComputeSHMat::IrradianceComputeSHMat()
    {
        _paramBuffer = gIrradianceComputeSHParamDef.CreateBuffer();
    }

    void IrradianceComputeSHMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_COMPUTE_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_COMPUTE_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void IrradianceComputeSHMat::Execute(const SPtr<Texture>& source, UINT32 face, const SPtr<GpuBuffer>& output)
    {
        auto& props = source->GetProperties();
        UINT32 faceSize = props.GetWidth();
        assert(faceSize == props.GetHeight());

        Vector2I dispatchSize;
        dispatchSize.x = Math::DivideAndRoundUp(faceSize, TILE_WIDTH * PIXELS_PER_THREAD);
        dispatchSize.y = Math::DivideAndRoundUp(faceSize, TILE_HEIGHT * PIXELS_PER_THREAD);

        _params->SetTexture(GPT_COMPUTE_PROGRAM, "SourceMap", source);

        gIrradianceComputeSHParamDef.gCubeFace.Set(_paramBuffer, face);
        gIrradianceComputeSHParamDef.gFaceSize.Set(_paramBuffer, source->GetProperties().GetWidth());
        gIrradianceComputeSHParamDef.gDispatchSize.Set(_paramBuffer, dispatchSize);

        _params->SetBuffer(GPT_COMPUTE_PROGRAM, "Output", output);

        RenderAPI& rapi = RenderAPI::Instance();

        Bind();
        rapi.DispatchCompute(dispatchSize.x, dispatchSize.y);
    }

    SPtr<GpuBuffer> IrradianceComputeSHMat::CreateOutputBuffer(const SPtr<Texture>& source, UINT32& numCoeffSets)
    {
        auto& props = source->GetProperties();
        UINT32 faceSize = props.GetWidth();
        assert(faceSize == props.GetHeight());

        Vector2I dispatchSize;
        dispatchSize.x = Math::DivideAndRoundUp(faceSize, TILE_WIDTH * PIXELS_PER_THREAD);
        dispatchSize.y = Math::DivideAndRoundUp(faceSize, TILE_HEIGHT * PIXELS_PER_THREAD);

        numCoeffSets = dispatchSize.x * dispatchSize.y * 6;

        GPU_BUFFER_DESC bufferDesc;
        bufferDesc.Type = GBT_STRUCTURED;
        bufferDesc.ElementCount = numCoeffSets;
        bufferDesc.Format = BF_UNKNOWN;
        bufferDesc.Usage = GBU_LOADSTORE;
        bufferDesc.ElementSize = sizeof(SHCoeffsAndWeight5);
        bufferDesc.DebugName = "[TEMP] Irradiance Compute SH";

        return GpuBuffer::Create(bufferDesc);
    }

    IrradianceReduceSHParamDef gIrradianceReduceSHParamDef;

    IrradianceReduceSHMat::IrradianceReduceSHMat()
    {
        _paramBuffer = gIrradianceComputeSHParamDef.CreateBuffer();
    }

    void IrradianceReduceSHMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_COMPUTE_PROGRAM, "PerFrameBuffer", _paramBuffer);
    }

    void IrradianceReduceSHMat::Execute(const SPtr<GpuBuffer>& source, UINT32 numCoeffSets, const SPtr<Texture>& output, UINT32 outputIdx)
    {
        Vector2I outputCoords = IBLUtility::GetSHCoeffXYFromIdx(outputIdx, SH_ORDER);
        gIrradianceReduceSHParamDef.gOutputIdx.Set(_paramBuffer, outputCoords);
        gIrradianceReduceSHParamDef.gNumEntries.Set(_paramBuffer, numCoeffSets);

        _params->SetBuffer(GPT_COMPUTE_PROGRAM, "Input", source);
        _params->SetLoadStoreTexture(GPT_COMPUTE_PROGRAM, "Output", output);

        Bind();

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.DispatchCompute(1);
    }

    SPtr<Texture> IrradianceReduceSHMat::CreateOutputTexture(UINT32 numCoeffSets)
    {
        Vector2I size = IBLUtility::GetSHCoeffTextureSize(numCoeffSets, SH_ORDER);

        TEXTURE_DESC textureDesc;
        textureDesc.Width = (UINT32)size.x;
        textureDesc.Height = (UINT32)size.y;
        textureDesc.Format = PF_RGBA32F;
        textureDesc.Usage = TU_STATIC | TU_LOADSTORE;
        textureDesc.DebugName = "[TEMP] Irradiance Reduce SH";

        return Texture::CreatePtr(textureDesc);
    }

    IrradianceProjectSHParamDef gIrradianceProjectSHParamDef;

    IrradianceProjectSHMat::IrradianceProjectSHMat()
    {
        _paramBuffer = gIrradianceProjectSHParamDef.CreateBuffer();
    }

    void IrradianceProjectSHMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
    }

    void IrradianceProjectSHMat::Execute(const SPtr<Texture>& shCoeffs, UINT32 face, const SPtr<RenderTarget>& target)
    {
        gIrradianceProjectSHParamDef.gCubeFace.Set(_paramBuffer, face);

        _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", shCoeffs);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(target);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }

    void RenderManIBLUtility::FilterCubemapForSpecular(const SPtr<Texture>& cubemap, const SPtr<Texture>& scratch) const
    {
        auto& props = cubemap->GetProperties();

        SPtr<Texture> scratchCubemap = scratch;
        if (scratchCubemap == nullptr)
        {
            TEXTURE_DESC cubemapDesc;
            cubemapDesc.Type = TEX_TYPE_CUBE_MAP;
            cubemapDesc.Format = props.GetFormat();
            cubemapDesc.Width = props.GetWidth();
            cubemapDesc.Height = props.GetHeight();
            cubemapDesc.NumMips = PixelUtil::GetMaxMipmaps(cubemapDesc.Width, cubemapDesc.Height, 1);
            cubemapDesc.Usage = TU_STATIC | TU_RENDERTARGET;
            cubemapDesc.DebugName = "[MIP][TEMP] " + props.GetDebugName();
            cubemapDesc.HwGamma = cubemap->GetProperties().IsHardwareGammaEnabled();

            scratchCubemap = Texture::CreatePtr(cubemapDesc);
        }

        // We sample the cubemaps using importance sampling to generate roughness
        UINT32 numMips = props.GetNumMipmaps() + 1;

        // Before importance sampling the cubemaps we first create box filtered versions for each mip level. This helps fix
        // the aliasing artifacts that would otherwise be noticeable on importance sampled cubemaps. The aliasing happens
        // because:
        //  1. We use the same random samples for all pixels, which appears to duplicate reflections instead of creating
        //     noise, which is usually more acceptable
        //  2. Even if we were to use fully random samples we would need a lot to avoid noticeable noise, which isn't
        //     practical

        // Copy base mip level to scratch cubemap
        for (UINT32 face = 0; face < 6; face++)
        {
            TEXTURE_COPY_DESC copyDesc;
            copyDesc.SrcFace = face;
            copyDesc.DstFace = face;

            cubemap->Copy(scratchCubemap, copyDesc);
        }

        // Fill out remaining scratch mip levels by downsampling
        for (UINT32 mip = 1; mip < numMips; mip++)
        {
            UINT32 sourceMip = mip - 1;
            DownsampleCubemap(scratchCubemap, sourceMip, scratchCubemap, mip);
        }

        // Importance sample
        for (UINT32 mip = 1; mip < numMips; mip++)
        {
            for (UINT32 face = 0; face < 6; face++)
            {
                RENDER_TEXTURE_DESC cubeFaceRTDesc;
                cubeFaceRTDesc.ColorSurfaces[0].Tex = cubemap;
                cubeFaceRTDesc.ColorSurfaces[0].Face = face;
                cubeFaceRTDesc.ColorSurfaces[0].NumFaces = 1;
                cubeFaceRTDesc.ColorSurfaces[0].MipLevel = mip;

                SPtr<RenderTarget> target = RenderTexture::Create(cubeFaceRTDesc);

                ReflectionCubeImportanceSampleMat* material = ReflectionCubeImportanceSampleMat::Get();
                material->Execute(scratchCubemap, face, mip, target);
            }
        }

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(nullptr);
    }

    void RenderManIBLUtility::FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output) const
    {
        SPtr<Texture> coeffTexture;

        IrradianceComputeSHMat* shCompute = IrradianceComputeSHMat::Get();
        IrradianceReduceSHMat* shReduce = IrradianceReduceSHMat::Get();

        UINT32 numCoeffSets;
        SPtr<GpuBuffer> coeffSetBuffer = shCompute->CreateOutputBuffer(cubemap, numCoeffSets);
        for (UINT32 face = 0; face < 6; face++)
            shCompute->Execute(cubemap, face, coeffSetBuffer);

        coeffTexture = shReduce->CreateOutputTexture(1);
        shReduce->Execute(coeffSetBuffer, numCoeffSets, coeffTexture, 0);

        IrradianceProjectSHMat* shProject = IrradianceProjectSHMat::Get();
        for (UINT32 face = 0; face < 6; face++)
        {
            RENDER_TEXTURE_DESC cubeFaceRTDesc;
            cubeFaceRTDesc.ColorSurfaces[0].Tex = output;
            cubeFaceRTDesc.ColorSurfaces[0].Face = face;
            cubeFaceRTDesc.ColorSurfaces[0].NumFaces = 1;
            cubeFaceRTDesc.ColorSurfaces[0].MipLevel = 0;

            SPtr<RenderTarget> target = RenderTexture::Create(cubeFaceRTDesc);
            shProject->Execute(coeffTexture, face, target);
        }
    }

    void RenderManIBLUtility::FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output,
        UINT32 outputIdx) const
    {
        IrradianceComputeSHMat* shCompute = IrradianceComputeSHMat::Get();
        IrradianceReduceSHMat* shReduce = IrradianceReduceSHMat::Get();

        UINT32 numCoeffSets;
        SPtr<GpuBuffer> coeffSetBuffer = shCompute->CreateOutputBuffer(cubemap, numCoeffSets);
        for (UINT32 face = 0; face < 6; face++)
            shCompute->Execute(cubemap, face, coeffSetBuffer);

        shReduce->Execute(coeffSetBuffer, numCoeffSets, output, outputIdx);
    }

    void RenderManIBLUtility::ScaleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst,
        UINT32 dstMip) const
    {
        auto& srcProps = src->GetProperties();
        auto& dstProps = dst->GetProperties();

        SPtr<Texture> scratchTex = src;
        int sizeSrcLog2 = (int)log2((float)srcProps.GetWidth());
        int sizeDstLog2 = (int)log2((float)dstProps.GetWidth());

        int sizeLog2Diff = sizeSrcLog2 - sizeDstLog2;

        // If size difference is greater than one mip-level and we're downscaling, we need to generate intermediate mip
        // levels
        if (sizeLog2Diff > 1)
        {
            UINT32 mipSize = (UINT32)exp2((float)(sizeSrcLog2 - 1));
            UINT32 numDownsamples = sizeLog2Diff - 1;

            TEXTURE_DESC cubemapDesc;
            cubemapDesc.Type = TEX_TYPE_CUBE_MAP;
            cubemapDesc.Format = srcProps.GetFormat();
            cubemapDesc.Width = mipSize;
            cubemapDesc.Height = mipSize;
            cubemapDesc.NumMips = numDownsamples - 1;
            cubemapDesc.Usage = TU_STATIC | TU_RENDERTARGET;
            cubemapDesc.DebugName = "[MIP] " + srcProps.GetDebugName();
            cubemapDesc.HwGamma = src->GetProperties().IsHardwareGammaEnabled();

            scratchTex = Texture::CreatePtr(cubemapDesc);

            DownsampleCubemap(src, srcMip, scratchTex, 0);
            for (UINT32 i = 0; i < cubemapDesc.NumMips; i++)
                DownsampleCubemap(scratchTex, i, scratchTex, i + 1);

            srcMip = cubemapDesc.NumMips;
        }

        // Same size so just copy
        if (sizeSrcLog2 == sizeDstLog2)
        {
            for (UINT32 face = 0; face < 6; face++)
            {
                TEXTURE_COPY_DESC copyDesc;
                copyDesc.SrcFace = face;
                copyDesc.SrcMip = srcMip;
                copyDesc.DstFace = face;
                copyDesc.DstMip = dstMip;

                src->Copy(dst, copyDesc);
            }
        }
        else
        {
            DownsampleCubemap(scratchTex, srcMip, dst, dstMip);
        }
    }

    void RenderManIBLUtility::DownsampleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst,
        UINT32 dstMip)
    {
        for (UINT32 face = 0; face < 6; face++)
        {
            RENDER_TEXTURE_DESC cubeFaceRTDesc;
            cubeFaceRTDesc.ColorSurfaces[0].Tex = dst;
            cubeFaceRTDesc.ColorSurfaces[0].Face = face;
            cubeFaceRTDesc.ColorSurfaces[0].NumFaces = 1;
            cubeFaceRTDesc.ColorSurfaces[0].MipLevel = dstMip;

            SPtr<RenderTarget> target = RenderTexture::Create(cubeFaceRTDesc);

            TextureCubeDownsampleMat* material = TextureCubeDownsampleMat::Get();
            material->Execute(src, face, srcMip, target);
        }
    }
}
