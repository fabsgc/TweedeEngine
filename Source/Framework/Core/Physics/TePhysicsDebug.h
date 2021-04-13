#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Provides a physics debug interface. */
    class TE_CORE_EXPORT PhysicsDebug
    {
    public:
        /**
         * Draw debug information to renderTarget using given camera properties
         *
         * @param[in]	camera			camera to use
         * @param[in]	renderTarget	render target where you want to draw to
         */
        virtual void Draw(const SPtr<Camera>& camera, const SPtr<RenderTarget>& renderTarget) = 0;

        /**
         * Clear debug data (called once per frame)
         */
        virtual void Clear() = 0;
    };
}
