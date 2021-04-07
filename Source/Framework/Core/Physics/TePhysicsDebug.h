#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Provides a physics debug interface. */
    class TE_CORE_EXPORT PhysicsDebug
    {
    public:
        virtual void Draw(const SPtr<RenderTarget>& renderTarget) = 0;

        virtual void Clear() = 0;
    };
}
