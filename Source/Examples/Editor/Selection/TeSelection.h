#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "../TeEditorUtils.h"
#include "Image/TeTexture.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    class GpuPickingMat;
    class HudPickingMat;

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

        /** @copydoc GpuPicking::Draw */
        void DrawInternal(const HCamera& camera, const SPtr<SceneObject>& sceneObject, Vector<SPtr<CLight>>& lights, Vector<SPtr<CCamera>>& cameras);

        /** Specific way to draw a renderable */
        void DrawRenderable(const SPtr<CRenderable>& renderable);

        /** Specific way to draw a light */
        void DrawLights(const Vector<SPtr<CLight>>& light);

        /** Specific way to draw a camera */
        void DrawCameras(const Vector<SPtr<CCamera>>& camera);

    private:
        Editor::SelectionData& _selections;
        GpuPickingMat* _material;
        HudPickingMat* _hudMaterial;
    };
}
