#pragma once

#include "TeCorePrerequisites.h"
#include "Picking/TePickingUtils.h"

namespace te
{
    class HudPickingMat;

    class Hud
    {
    public:
        Hud();
        ~Hud() = default;

        /** Init context, shader */
        void Initialize();

        /** Take a list of component (lights, cameras and render billboard on top of previous render) */
        void Render(const HCamera& camera, const HSceneObject& root);

        /** Generate a list of all hud elements to draw by iterating through the scene graph */
        void GetHudElements(const HCamera& camera, const HSceneObject& sceneObject, Vector<PickingUtils::PerHudInstanceData>& instancedElements);

    private:
        HudPickingMat* _material;
        PickingUtils::HudInstanceBuffer _instanceBuffer;
    };
}