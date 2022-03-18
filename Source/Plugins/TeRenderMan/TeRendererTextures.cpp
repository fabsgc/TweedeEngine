#include "TeRendererTextures.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Image/TeColor.h"
#include "Math/TeMath.h"
#include "Image/TeTexture.h"
#include "Image/TePixelData.h"

namespace te
{
    // Reverse bits functions used for Hammersley sequence
    float ReverseBits(UINT32 bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

        return (float)(double(bits) / (double)0x100000000LL);
    }

    void HammersleySequence(UINT32 i, UINT32 count, float& e0, float& e1)
    {
        e0 = i / (float)count;
        e1 = ReverseBits(i);
    }

    Vector3 SphericalToCartesian(float cosTheta, float sinTheta, float phi)
    {
        Vector3 output;
        output.x = sinTheta * cos(phi);
        output.y = sinTheta * sin(phi);
        output.z = cosTheta;

        return output;
    }

    // Generates an angle in spherical coordinates, importance sampled for the specified roughness based on some uniformly
    // distributed random variables in range [0, 1].
    void ImportanceSampleGGX(float e0, float e1, float roughness4, float& cosTheta, float& phi)
    {
        // See GGXImportanceSample.nb for derivation (essentially, take base GGX, normalize it, generate PDF, split PDF into
        // marginal probability for theta and conditional probability for phi. Plug those into the CDF, invert it.)				
        cosTheta = sqrt((1.0f - e0) / (1.0f + (roughness4 - 1.0f) * e0));
        phi = 2.0f * Math::PI * e1;
    }

    float CalcMicrofacetShadowingSmithGGX(float roughness4, float NoV, float NoL)
    {
        // Note: See lighting shader for derivation. Includes microfacet BRDF divisor.
        float g1V = NoV + sqrt(NoV * (NoV - NoV * roughness4) + roughness4);
        float g1L = NoL + sqrt(NoL * (NoL - NoL * roughness4) + roughness4);
        return 1.0f / (g1V * g1L);
    }
    
    SPtr<Texture> GeneratePreIntegratedEnvBRDF()
    {
        TEXTURE_DESC desc;
        desc.Type = TEX_TYPE_2D;
        desc.Format = PF_RG16F;
        desc.Width = 128;
        desc.Height = 32;

        SPtr<Texture> texture = Texture::CreatePtr(desc);
        PixelData pixelData = texture->Lock(GBL_WRITE_ONLY_DISCARD);

        for (UINT32 y = 0; y < desc.Height; y++)
        {
            float roughness = (float)(y + 0.5f) / desc.Height;
            float m = roughness * roughness;
            float m2 = m*m;
            for (UINT32 x = 0; x < desc.Width; x++)
            {
                float NoV = (float)(x + 0.5f) / desc.Width;

                Vector3 V;
                V.x = sqrt(1.0f - NoV * NoV); // sine
                V.y = 0.0f;
                V.z = NoV;

                // These are the two integrals resulting from the second part of the split-sum approximation. Described in
                // Epic's 2013 paper:
                //    http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
                float scale = 0.0f;
                float offset = 0.0f;

                // We use the same importance sampling function we use for reflection cube importance sampling, only we
                // sample G and F, instead of D factors of the microfactet BRDF. See GGXImportanceSample.nb for derivation.
                constexpr UINT32 NumSamples = 128;
                for (UINT32 i = 0; i < NumSamples; i++)
                {
                    float e0, e1;
                    HammersleySequence(i, NumSamples, e0, e1);

                    float cosTheta, phi;
                    ImportanceSampleGGX(e0, e1, m2, cosTheta, phi);

                    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
                    Vector3 H = SphericalToCartesian(cosTheta, sinTheta, phi);
                    Vector3 L = 2.0f * Vector3::Dot(V, H) * H - V;

                    float VoH = std::max(Vector3::Dot(V, H), 0.0f);
                    float NoL = std::max(L.z, 0.0f); // N assumed (0, 0, 1)
                    float NoH = std::max(H.z, 0.0f); // N assumed (0, 0, 1)

                    // Set second part of the split sum integral is split into two parts:
                    //   F0*I[G * (1 - (1 - v.h)^5) * cos(theta)] + I[G * (1 - v.h)^5 * cos(theta)] (F0 * scale + bias)

                    // We calculate the fresnel scale (1 - (1 - v.h)^5) and bias ((1 - v.h)^5) parts
                    float fc = pow(1.0f - VoH, 5.0f);
                    float fresnelScale = 1.0f - fc;
                    float fresnelOffset = fc;

                    // multi scattering https://google.github.io/filament/Filament.html#listing_multiscatteriblevaluation
                    // float fresnelScale = 1.0f - fc;
                    // float fresnelOffset = fc;

                    // We calculate the G part
                    float G = CalcMicrofacetShadowingSmithGGX(m2, NoV, NoL);

                    // When we factor out G and F, then divide D by PDF, this is what's left
                    // Note: This is based on PDF: D * NoH / (4 * VoH). (4 * VoH) factor comes from the Jacobian of the
                    // transformation from half vector to light vector
                    float pdfFactor = 4.0f * VoH / NoH;

                    if (NoL > 0.0f)
                    {
                        scale += NoL * pdfFactor * G * fresnelScale;
                        offset += NoL * pdfFactor * G * fresnelOffset;
                    }
                }

                scale /= NumSamples;
                offset /= NumSamples;

                Color color;
                color.r = Math::Clamp01(scale);
                color.g = Math::Clamp01(offset);

                pixelData.SetColorAt(color, x, y);
            }
        }

        texture->Unlock();

        return texture;
    }

    SPtr<Texture> RendererTextures::PreIntegratedEnvGF;

    void RendererTextures::StartUp()
    {
        PreIntegratedEnvGF = GeneratePreIntegratedEnvBRDF();
    }

    void RendererTextures::ShutDown()
    {
        PreIntegratedEnvGF = nullptr;
    }
}
