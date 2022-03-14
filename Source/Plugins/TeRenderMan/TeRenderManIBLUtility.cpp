#include "TeRenderManIBLUtility.h"
#include "Image/TETexture.h"
#include "Renderer/TeRendererUtility.h"
#include "RenderAPI/TeGpuBuffer.h"
#include "TeRenderMan.h"

namespace te
{
    ReflectionCubeDownsampleParamDef gReflectionCubeDownsampleParamDef;

    ReflectionCubeDownsampleMat::ReflectionCubeDownsampleMat()
    {
        _paramBuffer = gReflectionCubeDownsampleParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void ReflectionCubeDownsampleMat::Execute(const SPtr<Texture>& source, UINT32 face, UINT32 mip,
        const SPtr<RenderTarget>& target)
    {
        gReflectionCubeDownsampleParamDef.gCubeFace.Set(_paramBuffer, face);

        const RenderAPICapabilities& caps = gCaps();
        if (caps.HasCapability(RSC_TEXTURE_VIEWS))
        {
            _params->SetTexture("SourceMap", source, TextureSurface(mip, 1, 0, 6));
            gReflectionCubeDownsampleParamDef.gMipLevel.Set(_paramBuffer, 0);
        }
        else
        {
            _params->SetTexture("SourceMap", source);
            gReflectionCubeDownsampleParamDef.gMipLevel.Set(_paramBuffer, mip);
        }

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
            cubemapDesc.DebugName = "[MIP] " + props.GetDebugName();
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

                //SPtr<RenderTarget> target = RenderTexture::Create(cubeFaceRTDesc);

                //ReflectionCubeImportanceSampleMat* material = ReflectionCubeImportanceSampleMat::Get();
                //material->Execute(scratchCubemap, face, mip, target);
            }
        }

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(nullptr);
    }

    void RenderManIBLUtility::FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output) const
    {
        // TODO PBR
    }

    void RenderManIBLUtility::FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output,
        UINT32 outputIdx) const
    {
        // TODO PBR
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

            ReflectionCubeDownsampleMat* material = ReflectionCubeDownsampleMat::Get();
            material->Execute(src, face, srcMip, target);
        }
    }

    void RenderManIBLUtility::FilterCubemapForIrradianceNonCompute(const SPtr<Texture>& cubemap, UINT32 outputIdx,
        const SPtr<RenderTexture>& output)
    {
        // TODO PBR
    }
}
