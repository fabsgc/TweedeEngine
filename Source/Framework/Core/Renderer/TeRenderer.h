#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class TE_CORE_EXPORT Renderer
    {
    public:
        Renderer();
        virtual ~Renderer() = default;
        virtual void Initialize() = 0;
        virtual void Update() = 0;
    };
}