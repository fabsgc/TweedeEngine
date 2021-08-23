#pragma once

#include "TeCorePrerequisites.h"
#include "Picking/TePicking.h"

namespace te
{
    class HudPickingMat;

    class EditorPicking : public Picking
    {
    public:
        EditorPicking();
        ~EditorPicking() = default;

        void Initialize() override;

    protected:
        /** @copydoc Picking::Draw */
        void Draw(const HCamera& camera, const HSceneObject& sceneObject) override;

        /** @copydoc Picking::Draw */
        void DrawInternal(const HCamera& camera, const HSceneObject& sceneObject, Vector<PickingUtils::PerHudInstanceData>& instancedElements) override;

    protected:
        HudPickingMat* _hudMaterial;
    };
}