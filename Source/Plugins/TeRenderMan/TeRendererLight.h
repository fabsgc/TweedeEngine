#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeLight.h"
#include "Renderer/TeRendererMaterial.h"
#include "RenderAPI/TeGpuPipelineParamInfo.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    struct LightData
    { 
        Vector3 Position;
        float BoundsRadius;
        Vector3 Direction;
        float Luminance;
        Vector3 SpotAngles;
        float AttRadiusSqrdInv;
        Vector3 Color;
        float SrcRadius;
        Vector3 ShiftedLightPosition;
        float Padding;
    };

    /** Maximum number of lights that can influence an object when basic forward rendering is used. */
    static constexpr UINT32 STANDARD_FORWARD_MAX_NUM_LIGHTS = 16;

    /**	Renderer information specific to a single light. */
    class RendererLight
    {
    public:
        RendererLight(Light* light);

        /** Populates the structure with light parameters. */
        void GetParameters(LightData& output) const;

        /**
         * Populates the provided parameter block buffer with information about the light. Provided buffer's structure
         * must match PerLightParamDef.
         */
        void GetParameters(SPtr<GpuParamBlockBuffer>& buffer) const;

        /**
         * Calculates the light position that is shifted in order to account for area spot lights. For non-spot lights
         * this method will return normal light position. The position will be shifted back from the light direction,
         * magnitude of the shift depending on the source radius.
         */
        Vector3 GetShiftedLightPosition() const;

        Light* _internal;
    };

    /** Helper struct containing all parameters required for forward lighting. */
    struct ForwardLightingParams
    {
        // TODO
    };

    /**
     * Contains lights that are visible from a specific set of views, determined by scene information provided to
     * setLights().
     */
    class VisibleLightData
    {
    public:
        VisibleLightData();

        // TODO
    };

    TE_PARAM_BLOCK_BEGIN(LightsParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(LightData, gLights, STANDARD_FORWARD_MAX_NUM_LIGHTS)
    TE_PARAM_BLOCK_END

    extern LightsParamDef gLightsParamDef;
}
