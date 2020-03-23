#include "TeGaussianBlurMat.h"
#include "TeRendererUtility.h"
#include "Renderer/TeGpuResourcePool.h"

namespace te
{
    GaussianBlurParamDef gGaussianBlurParamDef;

    GaussianBlurMat::GaussianBlurMat()
    {
        _paramBuffer = gGaussianBlurParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void GaussianBlurMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, float filterSize,
        const Color& tint, const SPtr<Texture>& additive, bool isAdditive, UINT32 MSAACount)
    {
        gGaussianBlurParamDef.gMSAACount.Set(_paramBuffer, MSAACount, 0);
        gGaussianBlurParamDef.gIsAdditive.Set(_paramBuffer, 1, 0);

        const TextureProperties& srcProps = source->GetProperties();
        const RenderTextureProperties& dstProps = static_cast<const RenderTextureProperties&>(destination->GetProperties());

        POOLED_RENDER_TEXTURE_DESC tempTextureDesc = POOLED_RENDER_TEXTURE_DESC::Create2D(srcProps.GetFormat(),
            dstProps.Width, dstProps.Height, TU_RENDERTARGET, MSAACount);
        SPtr<PooledRenderTexture> tempTexture = gGpuResourcePool().Get(tempTextureDesc);

        // Horizontal pass
        {
            PopulateBuffer(_paramBuffer, Direction::DirHorizontal, source, filterSize, Color::White);

            if (MSAACount > 1) _params->SetTexture("SourceMapMS", source);
            else _params->SetTexture("SourceMap", source);

            if (isAdditive) _params->SetTexture("AdditiveMap", Texture::BLACK);

            RenderAPI& rapi = RenderAPI::Instance();
            rapi.SetRenderTarget(tempTexture->RenderTex);

            Bind();
            gRendererUtility().DrawScreenQuad();
        }

        // Vertical pass
        {
            PopulateBuffer(_paramBuffer, Direction::DirVertical, source, filterSize, tint);

            if (MSAACount > 1) _params->SetTexture("SourceMapMS", tempTexture->Tex);
            else _params->SetTexture("SourceMap", tempTexture->Tex);

            if (isAdditive)
            {
                if (additive) _params->SetTexture("AdditiveMap", additive);
                else _params->SetTexture("AdditiveMap", Texture::BLACK);
            }

            RenderAPI& rapi = RenderAPI::Instance();
            rapi.SetRenderTarget(destination);

            Bind();
            gRendererUtility().DrawScreenQuad();
        }
    }

    UINT32 GaussianBlurMat::CalcStdDistribution(float filterRadius, std::array<float, STANDARD_MAX_BLUR_SAMPLES>& weights,
        std::array<float, STANDARD_MAX_BLUR_SAMPLES>& offsets)
    {
        filterRadius = Math::Clamp(filterRadius, 0.00001f, (float)(STANDARD_MAX_BLUR_SAMPLES - 1));
        INT32 intFilterRadius = std::min(Math::CeilToInt(filterRadius), STANDARD_MAX_BLUR_SAMPLES - 1);

        // Note: Does not include the scaling factor since we normalize later anyway
        auto normalDistribution = [](int i, float scale)
        {
            // Higher value gives more weight to samples near the center
            constexpr float CENTER_BIAS = 30;

            // Mathematica visualization: Manipulate[Plot[E^(-0.5*centerBias*(Abs[x]*(1/radius))^2), {x, -radius, radius}],
            //	{centerBias, 1, 30}, {radius, 1, 72}]
            float samplePos = fabs((float)i) * scale;
            return exp(-0.5f * CENTER_BIAS * samplePos * samplePos);
        };

        // We make use of the hardware linear filtering, and therefore only generate half the number of samples.
        // The weights and the sampling location needs to be adjusted in order to get the same results as if we
        // perform two samples separately:
        //
        // Original formula is: t1*w1 + t2*w2
        // With hardware filtering it's: (t1 + (t2 - t1) * o) * w3
        //	Or expanded: t1*w3 - t1*o*w3 + t2*o*w3 = t1 * (w3 - o*w3) + t2 * (o*w3)
        //
        // These two need to equal, which means this follows:
        // w1 = w3 - o*w3
        // w2 = o*w3
        //
        // From the second equation get the offset o:
        // o = w2/w3
        //
        // From the first equation and o, get w3:
        // w1 = w3 - w2
        // w3 = w1 + w2

        float scale = 1.0f / filterRadius;
        UINT32 numSamples = 0;
        float totalWeight = 0.0f;
        for (int i = -intFilterRadius; i < intFilterRadius; i += 2)
        {
            float w1 = normalDistribution(i, scale);
            float w2 = normalDistribution(i + 1, scale);

            float w3 = w1 + w2;
            float o = (float)i + w2 / w3; // Relative to first sample

            weights[numSamples] = w3;
            offsets[numSamples] = o;

            numSamples++;
            totalWeight += w3;
        }

        // Special case for last weight, as it doesn't have a matching pair
        float w = normalDistribution(intFilterRadius, scale);
        weights[numSamples] = w;
        offsets[numSamples] = (float)(intFilterRadius - 1);

        numSamples++;
        totalWeight += w;

        // Normalize weights
        float invTotalWeight = 1.0f / totalWeight;
        for (UINT32 i = 0; i < numSamples; i++)
            weights[i] *= invTotalWeight;

        return numSamples;
    }

    float GaussianBlurMat::CalcKernelRadius(const SPtr<Texture>& source, float scale, Direction filterDir)
    {
        scale = Math::Clamp01(scale);

        UINT32 length;
        if (filterDir == Direction::DirHorizontal)
            length = source->GetProperties().GetWidth();
        else
            length = source->GetProperties().GetHeight();

        // Divide by two because we need the radius
        return std::min(length * scale / 2, (float)STANDARD_MAX_BLUR_SAMPLES - 1);
    }

    void GaussianBlurMat::PopulateBuffer(const SPtr<GpuParamBlockBuffer>& buffer, Direction direction,
        const SPtr<Texture>& source, float filterSize, const Color& tint)
    {
        const TextureProperties& srcProps = source->GetProperties();

        Vector2 invTexSize(1.0f / srcProps.GetWidth(), 1.0f / srcProps.GetHeight());

        std::array<float, STANDARD_MAX_BLUR_SAMPLES> sampleOffsets;
        std::array<float, STANDARD_MAX_BLUR_SAMPLES> sampleWeights;

        const float kernelRadius = CalcKernelRadius(source, filterSize, direction);
        const UINT32 numSamples = CalcStdDistribution(kernelRadius, sampleWeights, sampleOffsets);

        for (UINT32 i = 0; i < numSamples; ++i)
        {
            Vector4 weight(tint.r, tint.g, tint.b, tint.a);
            weight *= sampleWeights[i];

            gGaussianBlurParamDef.gSampleWeights.Set(buffer, weight, i);
        }

        UINT32 axis0 = direction == Direction::DirHorizontal ? 0 : 1;
        UINT32 axis1 = (axis0 + 1) % 2;

        for (UINT32 i = 0; i < (numSamples + 1) / 2; ++i)
        {
            UINT32 remainder = std::min(2U, numSamples - i * 2);

            Vector4 offset;
            offset[axis0] = sampleOffsets[i * 2 + 0] * invTexSize[axis0];
            offset[axis1] = 0.0f;

            if (remainder == 2)
            {
                offset[axis0 + 2] = sampleOffsets[i * 2 + 1] * invTexSize[axis0];
                offset[axis1 + 2] = 0.0f;
            }
            else
            {
                offset[axis0 + 2] = 0.0f;
                offset[axis1 + 2] = 0.0f;
            }

            gGaussianBlurParamDef.gSampleOffsets.Set(buffer, offset, i);
        }

        gGaussianBlurParamDef.gNumSamples.Set(buffer, numSamples);
    }
}
