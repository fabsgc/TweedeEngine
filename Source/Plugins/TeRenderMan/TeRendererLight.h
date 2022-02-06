#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeLight.h"

namespace te
{
    struct SceneInfo;
    class RendererViewGroup;

    /** Helper class used for manipulating the PerObject parameter buffer. */
    class PerLightsBuffer
    {
    public:
        /**
         * Updates the provided buffer with the data from the provided matrices.
         *
         *  @param[in]	lightNumber	  Number of lights to be rendered for this pass
         *  @param[in]	lights  	  Array of lights data
         */
        static void UpdatePerLights(const PerLightData* (&lights)[STANDARD_FORWARD_MAX_NUM_LIGHTS], UINT32 lightNumber);
    };

    /**	Renderer information specific to a single light. */
    class RendererLight
    {
    public:
        RendererLight(Light* light);
        ~RendererLight();

        /** Populates the structure with light parameters. */
        void GetParameters(PerLightData& output) const;

        Light* _internal;
    };

    /**
     * Contains lights that are visible from a specific set of views, determined by scene information provided to
     * SetLights().
     */
    class VisibleLightData
    {
    public:
        VisibleLightData();

        /**
         * Updates the internal buffers with a new set of lights. Before calling make sure that light visibility has
         * been calculated for the provided view group.
         */
        void Update(const SceneInfo& sceneInfo, const RendererViewGroup& viewGroup);

        /**
         * Scans the list of lights visible in the view frustum to find the ones influencing the object described by
         * the provided bounds. A maximum number of STANDARD_FORWARD_MAX_NUM_LIGHTS will be output. If there are more
         * influencing lights, only the most important ones will be returned.
         *
         * The lights will be output in the following order: directional, radial, spot. @p counts will contain the number
         * of directional lights (component 'x'), number of radial lights (component 'y') and number of spot lights
         * (component 'z');
         *
         * Update() must have been called with most recent scene/view information before calling this method.
         */
        void GatherInfluencingLights(const Bounds& bounds, const PerLightData* (&output)[STANDARD_FORWARD_MAX_NUM_LIGHTS],
            Vector3I& counts) const;

        /**
         * Scans the list of lights visible in the view frustum. A maximum number of STANDARD_FORWARD_MAX_NUM_LIGHTS will be output. 
         * If there are more influencing lights, only the most important ones will be returned.
         *
         * The lights will be output in the following order: directional, radial, spot. @p counts will contain the number
         * of directional lights (component 'x'), number of radial lights (component 'y') and number of spot lights
         * (component 'z');
         *
         * Update() must have been called with most recent scene/view information before calling this method.
         */
        void GatherLights(const PerLightData* (&output)[STANDARD_FORWARD_MAX_NUM_LIGHTS],
            Vector3I& counts) const;

        /** Returns the number of directional lights in the lights buffer. */
        UINT32 GetNumDirLights() const { return _numLights[0]; }

        /** Returns the number of radial point lights in the lights buffer. */
        UINT32 GetNumRadialLights() const { return _numLights[1]; }

        /** Returns the number of spot point lights in the lights buffer. */
        UINT32 GetNumSpotLights() const { return _numLights[2]; }

        /** Returns the number of visible lights of the specified type. */
        UINT32 GetNumLights(LightType type) const { return _numLights[(UINT32)type]; }

        /** Returns the number of visible shadowed lights of the specified type. */
        UINT32 GetNumShadowedLights(LightType type) const { return _numShadowedLights[(UINT32)type]; }

        /** Returns the number of visible unshadowed lights of the specified type. */
        UINT32 GetNumUnshadowedLights(LightType type) const { return _numLights[(UINT32)type] - _numShadowedLights[(UINT32)type]; }

        /** Returns a list of all visible lights of the specified type. */
        const Vector<const RendererLight*>& GetLights(LightType type) const { return _visibleLights[(UINT32)type]; }

    private:
        INT32 _numLights[(UINT32)LightType::Count];
        UINT32 _numShadowedLights[(UINT32)LightType::Count];

        // These are rebuilt every call to update()
        Vector<const RendererLight*> _visibleLights[(UINT32)LightType::Count];
        Vector<PerLightData> _visibleLightData;
    };
}
