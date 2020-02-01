#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeLight.h"

namespace te
{
    struct LightData
    { 
        Vector3 Position;
        Vector3 Direction;
    };

    /**	Renderer information specific to a single light. */
    class RendererLight
    {
    public:
        RendererLight(Light* light);

        /** Populates the structure with light parameters. */
        void GetParameters(LightData& output) const;

        Light* _internal;
    };
}
