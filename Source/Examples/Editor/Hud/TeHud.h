#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class HudMat;

    class Hud
    {
    public:
        Hud() = default;
        ~Hud() = default;

        /** Init context, shader */
        void Initialize();

        /** Take a list of component (lights, cameras and render billboard on top of previous render) */
        void Render();

    private:
        HudMat* _material;
    };
}