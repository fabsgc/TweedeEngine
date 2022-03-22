#include "TeRendererTextures.h"
#include "Math/TeMatrix3.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Image/TeColor.h"
#include "Math/TeMath.h"
#include "Image/TeTexture.h"
#include "Image/TePixelData.h"

namespace te
{
    struct DistributionSample
    {
        float SinTheta = 0.0f;
        float CosTheta = 0.0f;
        float Phi = 0.0f;
        float Pdf = 0.0f;
    };

    // Hammersley Points on the Hemisphere
    // CC BY 3.0 (Holger Dammertz)
    // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
    // with adapted interface
    float ReverseBits(UINT32 bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

        return (float)(double(bits) / (double)0x100000000LL);
    }

    // HammersleySequence describes a sequence of points in the 2d unit square [0,1)^2
    // that can be used for quasi Monte Carlo integration
    Vector2 HammersleySequence(UINT32 i, UINT32 count)
    {
        return Vector2(i / (float)count, ReverseBits(i));
    }

    Vector3 SphericalToCartesian(float cosTheta, float sinTheta, float phi)
    {
        Vector3 output;
        output.x = sinTheta * cos(phi);
        output.y = sinTheta * sin(phi);
        output.z = cosTheta;

        return output;
    }

    // From the filament docs. Geometric Shadowing function
    // https://google.github.io/filament/Filament.html#toc4.4.2
    float V_SmithGGXCorrelated(float NoV, float NoL, float roughness) 
    {
        float a2 = pow(roughness, 4.0f);
        float GGXV = NoL * sqrt(NoV * NoV * (1.0f - a2) + a2);
        float GGXL = NoV * sqrt(NoL * NoL * (1.0f - a2) + a2);
        return 0.5f / (GGXV + GGXL);
    }

    // https://github.com/google/filament/blob/master/shaders/src/brdf.fs#L136
    float V_Ashikhmin(float NdotL, float NdotV)
    {
        return Math::Clamp(1.0f / (4.0f * (NdotL + NdotV - NdotL * NdotV)), 0.0f, 1.0f);
    }

    float D_GGX(float NdotH, float roughness) 
    {
        float a = NdotH * roughness;
        float k = roughness / (1.0f - NdotH * NdotH + a * a);
        return k * k * (1.0f / Math::PI);
    }

    float D_Charlie(float sheenRoughness, float NdotH)
    {
        sheenRoughness = Math::Max(sheenRoughness, 0.000001f); //clamp (0,1]
        float invR = 1.0f / sheenRoughness;
        float cos2h = NdotH * NdotH;
        float sin2h = 1.0f - cos2h;
        return (2.0f + invR) * pow(sin2h, invR * 0.5f) / (2.0f * Math::PI);
    }

    // TBN generates a tangent bitangent normal coordinate frame from the normal
    // (the normal must be normalized)
    Matrix3 GenerateTBN(const Vector3& normal)
    {
        Vector3 bitangent = Vector3::UNIT_Y;

        float NdotUp = normal.Dot(Vector3::UNIT_Y);
        float epsilon = 0.0000001f;
        if (1.0 - abs(NdotUp) <= epsilon)
        {
            // Sampling +Y or -Y, so we need a more robust bitangent.
            if (NdotUp > 0.0)
            {
                bitangent = Vector3::UNIT_Z;
            }
            else
            {
                bitangent = -Vector3::UNIT_Z;
            }
        }

        Vector3 tangent = bitangent.Cross(normal);
        tangent.Normalize();
        bitangent = normal.Cross(tangent);

        return Matrix3(tangent, bitangent, normal);
    }

    // GGX microfacet distribution
    // https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.html
    // This implementation is based on https://bruop.github.io/ibl/,
    //  https://www.tobias-franke.eu/log/2014/03/30/notes_on_importance_sampling.html
    // and https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
    DistributionSample ImportanceSampleGGX(const Vector2& xi, float roughness)
    {
        DistributionSample ggx;
		
        // evaluate sampling equations
        float alpha = roughness * roughness;
        ggx.CosTheta = Math::Clamp01((1.0f - xi.y) / (1.0f + (alpha * alpha - 1.0f) * xi.y));
        ggx.SinTheta = sqrt(1.0f - ggx.CosTheta * ggx.CosTheta);
        ggx.Phi = 2.0f * Math::PI * xi.x;

        // evaluate GGX pdf (for half vector)
        ggx.Pdf = D_GGX(ggx.CosTheta, alpha);

        // Apply the Jacobian to obtain a pdf that is parameterized by l
        // see https://bruop.github.io/ibl/
        // Typically you'd have the following:
        // float pdf = D_GGX(NoH, roughness) * NoH / (4.0 * VoH);
        // but since V = N => VoH == NoH
        ggx.Pdf /= 4.0f;

        return ggx;
    }

    // Generates an angle in spherical coordinates, importance sampled for the specified roughness based on some uniformly
    // distributed random variables in range [0, 1].
    DistributionSample ImportanceSampleCharlie(const Vector2& xi, float sheenRoughness)
    {
        DistributionSample charlie;

        float alpha = Math::Clamp(sheenRoughness, 1e-5f, 1.0f);
        charlie.SinTheta = pow(xi.y, alpha / (2.0f * alpha + 1.0f));
        charlie.CosTheta = sqrt(1.0f - charlie.SinTheta * charlie.SinTheta);
        charlie.Phi = 2.0f * Math::PI * xi.x;

        // evaluate Charlie pdf (for half vector)
        charlie.Pdf = D_Charlie(alpha, charlie.CosTheta);

        // Apply the Jacobian to obtain a pdf that is parameterized by l
        charlie.Pdf /= 4.0;

        return charlie;
    }

    SPtr<Texture> GeneratePreIntegratedEnvBRDF(RendererTextures::DistributionMode mode)
    {
        TEXTURE_DESC desc;
        desc.Type = TEX_TYPE_2D;
        desc.Format = PF_RGBA32F;
        desc.Width = 128;
        desc.Height = 128;
        desc.DebugName = mode == RendererTextures::DistributionMode::GGX ? "GGX LUT" : "Charlie LUT";

        Vector3 N = Vector3::UNIT_Z;
        Matrix3 TBN = GenerateTBN(N);

        SPtr<Texture> texture = Texture::CreatePtr(desc);
        PixelData pixelData = texture->Lock(GBL_WRITE_ONLY_DISCARD);

        for (UINT32 y = 0; y < desc.Height; y++)
        {
            float roughness = (float)y / desc.Height;
            float m = roughness * roughness;
            for (UINT32 x = 0; x < desc.Width; x++)
            {
                float NoV = (float)x / desc.Width;

                Vector3 V;
                V.x = sqrt(1.0f - NoV * NoV); // sine
                V.y = 0.0f;
                V.z = NoV;

                // To make the LUT independant from the material's F0, which is part of the Fresnel term
                // when substituted by Schlick's approximation, we factor it out of the integral,
                // yielding to the form: F0 * I1 + I2
                // I1 and I2 are slighlty different in the Fresnel term, but both only depend on
                // NoL and roughness, so they are both numerically integrated and written into two channels.
                float A = 0.0f;
                float B = 0.0f;
                float C = 0.0f;

                // We use the same importance sampling function we use for reflection cube importance sampling, only we
                // sample G and F, instead of D factors of the microfactet BRDF.
                constexpr UINT32 NumSamples = 128;
                for (UINT32 i = 0; i < NumSamples; i++)
                {
                    DistributionSample dSample;
                    Vector2 xi = HammersleySequence(i, NumSamples);

                    if(mode == RendererTextures::DistributionMode::GGX)
                        dSample = ImportanceSampleGGX(xi, m);
                    else
                        dSample = ImportanceSampleCharlie(xi, m);

                    //Vector3 H = TBN.Multiply(SphericalToCartesian(dSample.CosTheta, dSample.SinTheta, dSample.Phi));
                    Vector3 H = SphericalToCartesian(dSample.CosTheta, dSample.SinTheta, dSample.Phi);
                    //Vector3 H = SphericalToCartesian(dSample.CosTheta, dSample.SinTheta, dSample.Phi);
                    Vector3 L = (-V) - 2 * N * Vector3::Dot(-V, N);

                    float VoH = std::max(Vector3::Dot(V, H), 0.0f);
                    float NoL = (L.z); // N assumed (0, 0, 1)
                    float NoH = (H.z); // N assumed (0, 0, 1)

                    if (NoL > 0.0f)
                    {
                        if (mode == RendererTextures::DistributionMode::GGX)
                        {
                            // LUT for GGX distribution.

                            // Taken from: https://bruop.github.io/ibl
                            // Shadertoy: https://www.shadertoy.com/view/3lXXDB
                            // Terms besides V are from the GGX PDF we're dividing by.

                            float v_pdf = V_SmithGGXCorrelated(NoV, NoL, roughness) * VoH * NoL / NoH;
                            float fc = pow(1.0f - VoH, 5.0f);
                            A += (1.0f - fc) * v_pdf;
                            B += fc * v_pdf;
                            C += 0.0f;
                        }
                        else
                        {
                            // LUT for Charlie distribution.
                            float sheenDistribution = D_Charlie(roughness, NoH);
                            float sheenVisibility = V_Ashikhmin(NoL, NoV);

                            A += 0.0f;
                            B += 0.0f;
                            C += sheenVisibility * sheenDistribution * NoL * VoH;
                        }
                    }
                }

                A /= NumSamples;
                B /= NumSamples;
                C /= NumSamples;

                // The PDF is simply pdf(v, h) -> NDF * <nh>.
                // To parametrize the PDF over l, use the Jacobian transform, yielding to: pdf(v, l) -> NDF * <nh> / 4<vh>
                // Since the BRDF divide through the PDF to be normalized, the 4 can be pulled out of the integral.
                Color color;
                color.r = Math::Clamp01(4.0f * A);
                color.g = Math::Clamp01(4.0f * B);
                color.b = Math::Clamp01(4.0f * 2.0f * Math::PI * C);

                pixelData.SetColorAt(color, x, y);
            }
        }

        texture->Unlock();

        return texture;
    }

    SPtr<Texture> RendererTextures::PreIntegratedEnvGF_GGX;
    SPtr<Texture> RendererTextures::PreIntegratedEnvGF_Charlie;

    void RendererTextures::StartUp()
    {
        PreIntegratedEnvGF_GGX = GeneratePreIntegratedEnvBRDF(RendererTextures::DistributionMode::GGX);
        PreIntegratedEnvGF_Charlie = GeneratePreIntegratedEnvBRDF(RendererTextures::DistributionMode::Charlie);
    }

    void RendererTextures::ShutDown()
    {
        PreIntegratedEnvGF_GGX = nullptr;
        PreIntegratedEnvGF_Charlie = nullptr;
    }
}
