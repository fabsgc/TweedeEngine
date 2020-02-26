#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeLight.h"
#include "Renderer/TeRendererMaterial.h"
#include "RenderAPI/TeGpuPipelineParamInfo.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    /** Helper class used for manipulating the PerObject parameter buffer. */
    class PerLightsBuffer
    {
    public:
        /**
         * Updates the provided buffer with the data from the provided matrices.
         *
         *  @param[in]	buffer	      Buffer which will be filled with data
         *  @param[in]	lightNumber	  Number of lights to be rendered for this pass
         *  @param[in]	lights  	  Vector of lights data
         */
        static void UpdatePerLights(SPtr<GpuParamBlockBuffer>& buffer, UINT8 lightNumber, Vector<LightData>& lights);
    };

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
}
