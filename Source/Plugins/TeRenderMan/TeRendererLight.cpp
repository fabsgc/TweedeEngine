#include "TeRendererLight.h"
#include "TeRendererView.h"
#include "TeRendererScene.h"

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
        const VisibilityInfo& visibility = viewGroup.GetVisibilityInfo();

        for (UINT32 i = 0; i < (UINT32)LightType::Count; i++)
            _visibleLights[i].clear();

        // Generate a list of lights and their GPU buffers
        UINT32 numDirLights = (UINT32)sceneInfo.DirectionalLights.size();
        for (UINT32 i = 0; i < numDirLights; i++)
            _visibleLights[(UINT32)LightType::Directional].push_back(&sceneInfo.DirectionalLights[i]);

        UINT32 numRadialLights = (UINT32)sceneInfo.RadialLights.size();
        for (UINT32 i = 0; i < numRadialLights; i++)
        {
            if (!visibility.RadialLights[i])
                continue;

            _visibleLights[(UINT32)LightType::Radial].push_back(&sceneInfo.RadialLights[i]);
        }

        UINT32 numSpotLights = (UINT32)sceneInfo.SpotLights.size();
        for (UINT32 i = 0; i < numSpotLights; i++)
        {
            if (!visibility.SpotLights[i])
                continue;

            _visibleLights[(UINT32)LightType::Spot].push_back(&sceneInfo.SpotLights[i]);
        }

        for (UINT32 i = 0; i < (UINT32)LightType::Count; i++)
            _numLights[i] = (UINT32)_visibleLights[i].size();

        // Partition all visible lights so that unshadowed ones come first
        auto partition = [](Vector<const RendererLight*>& entries)
        {
            UINT32 numUnshadowed = 0;
            int first = -1;
            for (UINT32 i = 0; i < (UINT32)entries.size(); ++i)
            {
                if (entries[i]->_internal->GetCastsShadow())
                {
                    first = i;
                    break;
                }
                else
                    ++numUnshadowed;
            }

            if (first != -1)
            {
                for (UINT32 i = first + 1; i < (UINT32)entries.size(); ++i)
                {
                    if (!entries[i]->_internal->GetCastsShadow())
                    {
                        std::swap(entries[i], entries[first]);
                        ++numUnshadowed;
                    }
                }
            }

            return numUnshadowed;
        };

        for (UINT32 i = 0; i < (UINT32)LightType::Count; i++)
            _numShadowedLights[i] = _numLights[i] - partition(_visibleLights[i]);

        // Generate light data to initialize the GPU buffer with
        _visibleLightData.clear();
        for (auto& lightsPerType : _visibleLights)
        {
            for (auto& entry : lightsPerType)
            {
                _visibleLightData.push_back(LightData());
                entry->GetParameters(_visibleLightData.back());
            }
        }
    }

    void VisibleLightData::GatherInfluencingLights(const Bounds& bounds,
        const LightData* (&output)[STANDARD_FORWARD_MAX_NUM_LIGHTS], Vector3I& counts) const
    {
        // TODO
    }
}
