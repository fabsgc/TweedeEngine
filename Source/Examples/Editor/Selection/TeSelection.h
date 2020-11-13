#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "../TeEditorUtils.h"
#include "TeSelectionUtils.h"
#include "Image/TeTexture.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    class SelectionMat;
    class HudSelectionMat;

    class Selection
    {
    public:
        Selection();
        ~Selection() = default;

        /** Init context, shader */
        void Initialize();

        /** Render all selected elements (renderables, lights and cameras) */
        void Render(const HCamera& camera, const EditorUtils::RenderWindowData& viewportData);

    private:
        /** Recursive method to draw components under a sceneObject */
        void Draw(const HCamera& camera, const EditorUtils::RenderWindowData& viewportData);

        /** @copydoc Picking::Draw */
        void DrawInternal(const HCamera& camera, const SPtr<SceneObject>& sceneObject, Vector<SelectionUtils::PerHudInstanceData>& instancedElements);

        /** Specific way to draw a renderable */
        void DrawRenderable(const SPtr<CRenderable>& renderable);

    private:
        Editor::SelectionData& _selections;
        SelectionMat* _material;
        HudSelectionMat* _hudMaterial;
        SelectionUtils::HudInstanceBuffer _instanceBuffer;
    };
}
