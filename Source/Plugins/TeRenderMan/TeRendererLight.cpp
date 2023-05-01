#include "TeRendererLight.h"
#include "TeRendererView.h"
#include "TeRendererScene.h"

namespace te
{
    PerLightsParamDef gPerLightsParamDef;
    SPtr<GpuParamBlockBuffer> gPerLightsParamBuffer;

    void PerLightsBuffer::UpdatePerLights(const PerLightData* (&lights)[STANDARD_FORWARD_MAX_NUM_LIGHTS], UINT32 lightNumber)
    {
        if (!gPerLightsParamBuffer)
            gPerLightsParamBuffer = gPerLightsParamDef.CreateBuffer();

        gPerLightsParamDef.gLightsNumber.Set(gPerLightsParamBuffer, lightNumber);

        for (size_t i = 0; i < lightNumber; i++)
            gPerLightsParamDef.gLights.Set(gPerLightsParamBuffer, const_cast<PerLightData&>(*lights[i]), (UINT32)i);
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
        case LightType::Directional:
            type = 0;
            break;
        case LightType::Radial:
            type = 1;
            break;
        case LightType::Spot:
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
        output.CastShadows = _internal->GetCastShadows() ? 1 : 0;
        output.Type = type;
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
        {
            if (!visibility.DirectionalLights[i])
                continue;

            _visibleLights[(UINT32)LightType::Directional].push_back(&sceneInfo.DirectionalLights[i]);
        }
            

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

        for (UINT32 i = 0; i < (UINT32)LightType::Count; i++)
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

    void VisibleLightData::GatherInfluencingLights(const Bounds& bounds,
        const PerLightData* (&output)[STANDARD_FORWARD_MAX_NUM_LIGHTS], Vector3I& counts) const
    {
        UINT32 outputIndices[STANDARD_FORWARD_MAX_NUM_LIGHTS];
        UINT32 numInfluencingLights = 0;

        UINT32 numDirLights = GetNumDirLights();
        for (UINT32 i = 0; i < numDirLights; i++)
        {
            if (numInfluencingLights >= STANDARD_FORWARD_MAX_NUM_LIGHTS)
                return;

            outputIndices[numInfluencingLights] = i;
            numInfluencingLights++;
        }

        UINT32 pointLightOffset = numInfluencingLights;

        float distances[STANDARD_FORWARD_MAX_NUM_LIGHTS];
        for (UINT32 i = 0; i < STANDARD_FORWARD_MAX_NUM_LIGHTS; i++)
            distances[i] = std::numeric_limits<float>::max();

        // Note: This is an ad-hoc way of evaluating light influence, a better way might be wanted
        UINT32 numLights = (UINT32)_visibleLightData.size();
        UINT32 furthestLightIdx = (UINT32)-1;
        float furthestDistance = 0.0f;
        for (UINT32 j = numDirLights; j < numLights; j++)
        {
            const PerLightData* lightData = &_visibleLightData[j];

            Sphere lightSphere(lightData->Position, lightData->BoundsRadius);
            if (bounds.GetSphere().Intersects(lightSphere))
            {
                float distance = bounds.GetSphere().GetCenter().SquaredDistance(lightData->Position);

                // See where in the array can we fit the light
                if (numInfluencingLights < STANDARD_FORWARD_MAX_NUM_LIGHTS)
                {
                    outputIndices[numInfluencingLights] = j;
                    distances[numInfluencingLights] = distance;

                    if (distance > furthestDistance)
                    {
                        furthestLightIdx = numInfluencingLights;
                        furthestDistance = distance;
                    }

                    numInfluencingLights++;
                }
                else if (distance < furthestDistance)
                {
                    outputIndices[furthestLightIdx] = j;
                    distances[furthestLightIdx] = distance;

                    furthestDistance = distance;
                    for (UINT32 k = 0; k < STANDARD_FORWARD_MAX_NUM_LIGHTS; k++)
                    {
                        if (distances[k] > furthestDistance)
                        {
                            furthestDistance = distances[k];
                            furthestLightIdx = k;
                        }
                    }
                }
            }
        }

        // Output actual light data, sorted by type
        counts = Vector3I(0, 0, 0);

        for (UINT32 i = 0; i < pointLightOffset; i++)
        {
            output[i] = &_visibleLightData[outputIndices[i]];
            counts.x += 1;
        }

        UINT32 outputIdx = pointLightOffset;
        UINT32 spotLightIdx = GetNumDirLights() + GetNumRadialLights();
        for (UINT32 i = pointLightOffset; i < numInfluencingLights; i++)
        {
            bool isSpot = outputIndices[i] >= spotLightIdx;
            if (isSpot)
                continue;

            output[outputIdx++] = &_visibleLightData[outputIndices[i]];
            counts.y += 1;
        }

        for (UINT32 i = pointLightOffset; i < numInfluencingLights; i++)
        {
            bool isSpot = outputIndices[i] >= spotLightIdx;
            if (!isSpot)
                continue;

            output[outputIdx++] = &_visibleLightData[outputIndices[i]];
            counts.z += 1;
        }
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
