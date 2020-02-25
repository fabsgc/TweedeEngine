#include "TeRendererLight.h"

namespace te
{
    static const UINT32 LIGHT_DATA_BUFFER_INCREMENT = 16 * sizeof(LightData);

    RendererLight::RendererLight(Light* light)
        : _internal(light)
    { }

    void RendererLight::GetParameters(LightData& output) const
    {
        // TODO
    }

    void RendererLight::GetParameters(SPtr<GpuParamBlockBuffer>& buffer) const
    {
        // TODO
    }

    Vector3 RendererLight::GetShiftedLightPosition() const
    {
        const Transform& tfrm = _internal->GetTransform();
        Vector3 direction = -tfrm.GetRotation().ZAxis();

        // Create position for fake attenuation for area spot lights (with disc center)
        if (_internal->GetType() == LightType::Spot)
            return tfrm.GetPosition() - direction * (_internal->GetSourceRadius() / Math::Tan(_internal->GetSpotAngle() * 0.5f));
        else
            return tfrm.GetPosition();
    }

    VisibleLightData::VisibleLightData()
    { }

    LightsParamDef gLightsParamDef;
}
