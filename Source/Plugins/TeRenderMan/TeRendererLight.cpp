#include "TeRendererLight.h"

namespace te
{
    PerLightsParamDef gPerLightsParamDef;
    SPtr<GpuParamBlockBuffer> gPerLightsParamBuffer;

    static const UINT32 LIGHT_DATA_BUFFER_INCREMENT = 16 * sizeof(LightData);

    void PerLightsBuffer::UpdatePerLights(SPtr<GpuParamBlockBuffer>& buffer, UINT8 lightNumber, Vector<LightData>& lights)
    {
        if (!gPerLightsParamBuffer)
            gPerLightsParamBuffer = gPerLightsParamDef.CreateBuffer();
    }

    RendererLight::RendererLight(Light* light)
        : _internal(light)
    { }

    void RendererLight::GetParameters(LightData& output) const
    {
        Radian spotAngle = Math::Clamp(_internal->GetSpotAngle() * 0.5f, Degree(0), Degree(89));
        Color color = _internal->GetColor();

        const Transform& tfrm = _internal->GetTransform();
        output.Position = tfrm.GetPosition();
        output.Direction = -tfrm.GetRotation().ZAxis();
        output.Intensity = _internal->GetIntensity();
        output.SpotAngles.x = spotAngle.ValueRadians();
        output.SpotAngles.y = Math::Cos(output.SpotAngles.x);
        output.SpotAngles.z = 1.0f / std::max(1.0f - output.SpotAngles.y, 0.001f);
        output.AttenuationRadius = _internal->GetAttenuationRadius();
        output.Color = Vector3(color.r, color.g, color.b);
    }

    VisibleLightData::VisibleLightData()
        : _numLights { }
        , _numShadowedLights { }
    { }

    void VisibleLightData::Update(const SceneInfo& sceneInfo, const RendererViewGroup& viewGroup)
    {
        // TODO
    }

    void VisibleLightData::GatherInfluencingLights(const Bounds& bounds,
        const LightData* (&output)[STANDARD_FORWARD_MAX_NUM_LIGHTS], Vector3I& counts) const
    {
        // TODO
    }
}
