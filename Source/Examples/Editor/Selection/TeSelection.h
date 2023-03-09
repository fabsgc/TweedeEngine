#pragma once

#include "TeCorePrerequisites.h"

#include "../TeEditor.h"
#include "Picking/TePickingUtils.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    class SelectionMat;
    class SelectionBlitMat;
    class HudSelectionMat;
    struct PooledRenderTexture;

    class Selection
    {
    public:
        Selection();
        ~Selection();

        /** Init context, shader */
        void Initialize();

        /** Render all selected elements (renderables, lights and cameras) */
        void Render(const HCamera& camera, const RendererUtility::RenderTextureData& viewportData);

    private:
        /** 
         * Recursive method to draw components under a sceneObject 
         * HUD elements will be directly drawn to viewportData.RenderTex
         * Renderables will be first drawn on a separate texture
         */
        void Draw(const HCamera& camera, const RendererUtility::RenderTextureData& viewportData);

        /** @copydoc Picking::Draw */
        void DrawInternal(const HCamera& camera, const SPtr<SceneObject>& sceneObject, Vector<PickingUtils::PerHudInstanceData>& instancedElements,
            const RendererUtility::RenderTextureData& viewportData
        );

        /** Specific way to draw a renderable */
        void DrawRenderable(const SPtr<CRenderable>& renderable, const RendererUtility::RenderTextureData& viewportData);

        /** Renderables drawn on separate texture will be blurred and then blit on viewportData.RenderTex */
        void OutlineRenderables(const RendererUtility::RenderTextureData& viewportData);

        /** Create texture for selection highlight effect */
        void CreateRenderTexture(SPtr<RendererUtility::RenderTextureData>& renderTexture, UINT32 width, UINT32 height);

        /** Release texture for selection highlight effect */
        void ReleaseRenderTexture(SPtr<RendererUtility::RenderTextureData>& renderTexture);

    private:
        Editor::SelectionData& _selections;
        PickingUtils::HudInstanceBuffer _instanceBuffer;

        SelectionMat* _selectionMat;
        GaussianBlurMat* _selectionBlurMat;
        SelectionBlitMat* _selectionBlitMat;
        HudSelectionMat* _hudMat;

        bool _needOutline = false;

        SPtr<RendererUtility::RenderTextureData> _renderablesOutput;
        SPtr<RendererUtility::RenderTextureData> _blurredRenderablesOutput;
        SPtr<RendererUtility::RenderTextureData> _outlineRenderablesOutput;
    };
}
