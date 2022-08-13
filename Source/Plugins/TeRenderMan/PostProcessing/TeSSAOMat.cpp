#include "TeSSAOMat.h"

#include "Renderer/TeRendererUtility.h"

namespace te
{
    SSAOParamDef gSSAOParamDef;

    SSAOMat::SSAOMat()
    { 
        _paramBuffer = gSSAOParamDef.CreateBuffer();
    }

    void SSAOMat::Initialize()
    {
        //_params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        //_params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearClampedSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
        //_params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void SSAOMat::Execute(const RendererView& view, const SSAOTextureInputs& textures, const SPtr<RenderTexture>& destination, 
        const AmbientOcclusionSettings& settings, bool upSample, bool finalPass, UINT32 quality)
	{
        // Scale that can be used to adjust how quickly does AO radius increase with downsampled AO. This yields a very
        // small AO radius at highest level, and very large radius at lowest level
        static const float DOWNSAMPLE_SCALE = 4.0f;

        const RendererViewProperties& viewProps = view.GetProperties();
        const RenderTargetProperties& rtProps = destination->GetProperties();

        Vector2 tanHalfFOV;
        tanHalfFOV.x = 1.0f / viewProps.ProjTransform[0][0];
        tanHalfFOV.y = 1.0f / viewProps.ProjTransform[1][1];

        float cotHalfFOV = viewProps.ProjTransform[0][0];

        // Downsampled AO uses a larger AO radius (in higher resolutions this would cause too much cache trashing). This
        // means if only full res AO is used, then only AO from nearby geometry will be calculated.
        float viewScale = viewProps.Target.ViewRect.width / (float)rtProps.Width;

        // Ramp up the radius exponentially. c^log2(x) function chosen arbitrarily, as it ramps up the radius in a nice way
        float scale = pow(DOWNSAMPLE_SCALE, Math::Log2(viewScale));

        // Determine maximum radius scale (division by 4 because we don't downsample more than quarter-size)
        float maxScale = pow(DOWNSAMPLE_SCALE, Math::Log2(4.0f));

        // Normalize the scale in [0, 1] range
        scale /= maxScale;

        float radius = settings.Radius * scale;

        // Factors used for scaling the AO contribution with range
        Vector2 fadeMultiplyAdd;
        fadeMultiplyAdd.x = 1.0f / settings.FadeRange;
        fadeMultiplyAdd.y = -settings.FadeDistance / settings.FadeRange;

        gSSAOParamDef.gSampleRadius.Set(_paramBuffer, radius);
        gSSAOParamDef.gCotHalfFOV.Set(_paramBuffer, cotHalfFOV);
        gSSAOParamDef.gTanHalfFOV.Set(_paramBuffer, tanHalfFOV);
        gSSAOParamDef.gWorldSpaceRadiusMask.Set(_paramBuffer, 1.0f);
        gSSAOParamDef.gBias.Set(_paramBuffer, (settings.Bias * viewScale) / 1000.0f);
        gSSAOParamDef.gFadeMultiplyAdd.Set(_paramBuffer, fadeMultiplyAdd);
        gSSAOParamDef.gPower.Set(_paramBuffer, settings.Power);
        gSSAOParamDef.gIntensity.Set(_paramBuffer, settings.Intensity);

        if (upSample)
        {
            const TextureProperties& props = textures.AODownsampled->GetProperties();

            Vector2 downsampledPixelSize;
            downsampledPixelSize.x = 1.0f / props.GetWidth();
            downsampledPixelSize.y = 1.0f / props.GetHeight();

            gSSAOParamDef.gDownsampledPixelSize.Set(_paramBuffer, downsampledPixelSize);
        }

        // Generate a scale which we need to use in order to achieve tiling
        const TextureProperties& rndProps = textures.RandomRotations->GetProperties();
        UINT32 rndWidth = rndProps.GetWidth();
        UINT32 rndHeight = rndProps.GetHeight();

        //// Multiple of random texture size, rounded up
        UINT32 scaleWidth = (rtProps.Width + rndWidth - 1) / rndWidth;
        UINT32 scaleHeight = (rtProps.Height + rndHeight - 1) / rndHeight;

        Vector2 randomTileScale((float)scaleWidth, (float)scaleHeight);
        gSSAOParamDef.gRandomTileScale.Set(_paramBuffer, randomTileScale);

        //_params->SetTexture(GPT_PIXEL_PROGRAM, "SetupAOMap", textures.AOSetup);

        if (finalPass)
        {
            //_params->SetTexture(GPT_PIXEL_PROGRAM, "DepthMap", textures.SceneDepth);
            //_params->SetTexture(GPT_PIXEL_PROGRAM, "NormalsMap", textures.SceneNormals);
        }

        //if (upSample)
            //_params->SetTexture(GPT_PIXEL_PROGRAM, "DownsampledAOMap", textures.AODownsampled);

        //_params->SetTexture(GPT_PIXEL_PROGRAM, "RandomMap", textures.RandomRotations);

        SPtr<GpuParamBlockBuffer> perView = view.GetPerViewBuffer();
        //_params->SetParamBlockBuffer("PerCameraBuffer", perView);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        //Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
