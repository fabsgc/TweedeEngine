#include "TeRendererLight.h"
#include "TeRendererView.h"
#include "TeRendererScene.h"

namespace te
{
    PerLightsParamDef gPerLightsParamDef;
    SPtr<GpuParamBlockBuffer> gPerLightsParamBuffer;

    void PerLightsBuffer::UpdatePerLights(const InfluencingLight* lights, const UINT32* shadowSlots, UINT32 lightNumber)
    {
        if (!gPerLightsParamBuffer)
            gPerLightsParamBuffer = gPerLightsParamDef.CreateBuffer();

        gPerLightsParamDef.gLightsNumber.Set(gPerLightsParamBuffer, lightNumber);

        for (UINT32 i = 0; i < lightNumber; i++)
        {
            PerLightData data = *lights[i].Data;
            data.ShadowSlot = shadowSlots[i];
            gPerLightsParamDef.gLights.Set(gPerLightsParamBuffer, data, i);
        }
    }

    RendererLight::RendererLight(Light* light)
        : _internal(light)
    { }

    RendererLight::~RendererLight()
    { }

    void RendererLight::GetParameters(PerLightData& output) const
    {
        Radian spotAngle = Math::Clamp(_internal->GetSpotAngle() * 0.5f, Degree(0), Degree(89));
        Color color = _internal->GetColor();

        UINT32 type = 0;
        switch (_internal->GetType())
        {
        case Light::Type::Directional:
            type = 0;
            break;
        case Light::Type::Radial:
            type = 1;
            break;
        case Light::Type::Spot:
            type = 2;
            break;
        default:
            break;
        }

        const Transform& tfrm = _internal->GetTransform();
        output.Position = tfrm.GetPosition();
        output.Direction = -tfrm.GetRotation().ZAxis();
        output.Intensity = _internal->GetIntensity();
        output.SpotAngles.x = spotAngle.ValueRadians();
        output.SpotAngles.y = Math::Cos(output.SpotAngles.x);
        output.SpotAngles.z = 1.0f / std::max(1.0f - output.SpotAngles.y, 0.001f);
        output.Color = color.GetAsVector3();
        output.BoundsRadius = _internal->GetBounds().GetRadius();
        output.ShadowSlot = 0;
        output.Type = type;
    }

    Vector3 RendererLight::GetShiftedLightPosition() const
    {
        const Transform& tfrm = _internal->GetTransform();
        Vector3 direction = -tfrm.GetRotation().ZAxis();

        // Create position for fake attenuation for area spot lights (with disc center)
        if (_internal->GetType() == Light::Type::Spot)
            return tfrm.GetPosition() - direction * (1.f / Math::Tan(_internal->GetSpotAngle() * 0.5f));
        else
            return tfrm.GetPosition();
    }

    VisibleLightData::VisibleLightData()
        : _numLights { }
        , _numShadowedLights { }
    { }

    void VisibleLightData::Update(const SceneInfo& sceneInfo, const RendererViewGroup& viewGroup)
    {
        const VisibilityInfo& visibility = viewGroup.GetVisibilityInfo();

        for (UINT32 i = 0; i < (UINT32)Light::Type::Count; i++)
            _visibleLights[i].clear();

        // Generate a list of lights and their GPU buffers
        UINT32 numDirLights = (UINT32)sceneInfo.DirectionalLights.size();
        for (UINT32 i = 0; i < numDirLights; i++)
        {
            if (!visibility.DirectionalLights[i])
                continue;

            _visibleLights[(UINT32)Light::Type::Directional].push_back(&sceneInfo.DirectionalLights[i]);
        }

        UINT32 numRadialLights = (UINT32)sceneInfo.RadialLights.size();
        for (UINT32 i = 0; i < numRadialLights; i++)
        {
            if (!visibility.RadialLights[i])
                continue;

            _visibleLights[(UINT32)Light::Type::Radial].push_back(&sceneInfo.RadialLights[i]);
        }

        UINT32 numSpotLights = (UINT32)sceneInfo.SpotLights.size();
        for (UINT32 i = 0; i < numSpotLights; i++)
        {
            if (!visibility.SpotLights[i])
                continue;

            _visibleLights[(UINT32)Light::Type::Spot].push_back(&sceneInfo.SpotLights[i]);
        }

        for (UINT32 i = 0; i < (UINT32)Light::Type::Count; i++)
            _numLights[i] = (UINT32)_visibleLights[i].size();

        // Partition all visible lights so that unshadowed ones come first
        auto partition = [](Vector<const RendererLight*>& entries)
        {
            UINT32 numUnshadowed = 0;
            UINT32 first = static_cast<UINT32>(-1);
            for (UINT32 i = 0; i < (UINT32)entries.size(); ++i)
            {
                if (entries[i]->_internal->GetCastShadows())
                {
                    first = i;
                    break;
                }
                else
                    ++numUnshadowed;
            }

            if (first != static_cast<UINT32>(-1))
            {
                for (UINT32 i = first + 1; i < (UINT32)entries.size(); ++i)
                {
                    if (!entries[i]->_internal->GetCastShadows())
                    {
                        std::swap(entries[i], entries[first]);
                        ++numUnshadowed;
                    }
                }
            }

            return numUnshadowed;
        };

        for (UINT32 i = 0; i < (UINT32)Light::Type::Count; i++)
            _numShadowedLights[i] = _numLights[i] - partition(_visibleLights[i]);

        // Generate light data to initialize the GPU buffer with
        _visibleLightData.clear();
        for (auto& lightsPerType : _visibleLights)
        {
            for (auto& entry : lightsPerType)
            {
                _visibleLightData.push_back(PerLightData());
                entry->GetParameters(_visibleLightData.back());
            }
        }
    }

    UINT32 VisibleLightData::GatherInfluencingLights(const Bounds& bounds,
        InfluencingLight (&output)[STANDARD_FORWARD_MAX_NUM_LIGHTS]) const
    {
        const Sphere& receiver = bounds.GetSphere();
        UINT32 lightIndex = 0;
        UINT32 count = 0;
        for (const auto& lightsPerType : _visibleLights)
        {
            for (const RendererLight* source : lightsPerType)
            {
                const PerLightData& light = _visibleLightData[lightIndex++];
                float score = light.Intensity * std::max({light.Color.x, light.Color.y, light.Color.z});
                if (light.Type != (UINT32)Light::Type::Directional)
                {
                    if (!receiver.Intersects(source->_internal->GetBounds()))
                        continue;

                    const float distanceSquared = receiver.GetCenter().SquaredDistance(light.Position);
                    const float scale = light.Type == (UINT32)Light::Type::Radial ? 12.56637f : 3.14159f;
                    score /= scale * std::max(distanceSquared, 0.0001f);
                }

                if (score <= 0.0f || (count == STANDARD_FORWARD_MAX_NUM_LIGHTS && score <= output[count - 1].Score))
                    continue;

                if (count < STANDARD_FORWARD_MAX_NUM_LIGHTS)
                    ++count;

                UINT32 index = count - 1;
                while (index > 0 && score > output[index - 1].Score)
                {
                    output[index] = output[index - 1];
                    --index;
                }
                output[index] = { &light, source, score };
            }
        }

        return count;
    }
    
    void VisibleLightData::GatherLights(const PerLightData* (&output)[STANDARD_FORWARD_MAX_NUM_LIGHTS],
        Vector3I& counts) const
    {
        for (UINT32 i = 0; i < _visibleLightData.size(); i++)
        {
            output[i] = &_visibleLightData[i];

            if ((UINT32)_visibleLightData[i].Type == 0) counts.x++;
            if ((UINT32)_visibleLightData[i].Type == 1) counts.y++;
            if ((UINT32)_visibleLightData[i].Type == 2) counts.z++;
        }
    }
}
