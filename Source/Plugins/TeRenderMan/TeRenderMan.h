#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    class RenderMan: public Renderer
    {
    public:
        RenderMan();
        ~RenderMan();

        void Initialize() override;
        void Update() override;
    };
}