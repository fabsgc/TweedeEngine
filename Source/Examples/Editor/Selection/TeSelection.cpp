#include "TeSelection.h"

#include "TeSelectionMat.h"
#include "TeSelectionBlitMat.h"
#include "TeHudSelectionMat.h"
#include "Picking/TePickingUtils.h"
#include "Components/TeCCamera.h"
#include "Components/TeCRenderable.h"
#include "Renderer/TeRendererUtility.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/Materials/TeGaussianBlurMat.h"
#include "Renderer/TeGpuResourcePool.h"
#include "Scene/TeSceneObject.h"
#include "Mesh/TeMesh.h"

namespace te
{
    Selection::Selection()
        : _selections(gEditor().GetSelectionData())
        , _selectionMat(nullptr)
        , _selectionBlurMat(nullptr)
        , _selectionBlitMat(nullptr)
        , _hudMat(nullptr)
    { }

    Selection::~Selection()
    {
        ReleaseRenderTexture(_renderablesOutput);
        ReleaseRenderTexture(_blurredRenderablesOutput);
        ReleaseRenderTexture(_outlineRenderablesOutput);
    }

    void Selection::Initialize()
    {
        _selectionMat = SelectionMat::Get();
        _selectionBlurMat = GaussianBlurMat::Get();
        _selectionBlitMat = SelectionBlitMat::Get();
        _hudMat = HudSelectionMat::Get();

        PickingUtils::CreateHudInstanceBuffer(_instanceBuffer);
    }

    void Selection::Render(const HCamera& camera, const RendererUtility::RenderTextureData& viewportData)
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Mode::Game);
        if (isRunning)
            return;

        _needOutline = false;

        RenderAPI& rapi = RenderAPI::Instance();

        CreateRenderTexture(_renderablesOutput, viewportData.Width, viewportData.Height);
        CreateRenderTexture(_blurredRenderablesOutput, viewportData.Width / 2, viewportData.Height / 2);
        CreateRenderTexture(_outlineRenderablesOutput, viewportData.Width, viewportData.Height);

        rapi.PushMarker("[DRAW] Selection", Color(0.77f, 0.39f, 0.25f));

        rapi.SetRenderTarget(viewportData.RenderTex);
        rapi.ClearViewport(FBT_DEPTH, Color::Black);
        rapi.SetRenderTarget(nullptr);

        rapi.SetRenderTarget(_renderablesOutput->RenderTex);
        rapi.ClearViewport(FBT_DEPTH | FBT_COLOR, Color::Black);
        rapi.SetRenderTarget(nullptr);

        rapi.SetRenderTarget(_blurredRenderablesOutput->RenderTex);
        rapi.ClearViewport(FBT_DEPTH | FBT_COLOR, Color::Black);
        rapi.SetRenderTarget(nullptr);

        rapi.SetRenderTarget(_outlineRenderablesOutput->RenderTex);
        rapi.ClearViewport(FBT_DEPTH | FBT_COLOR, Color::Black);
        rapi.SetRenderTarget(nullptr);

        Draw(camera, viewportData);
        OutlineRenderables(viewportData);

        rapi.PopMarker();
    }

    void Selection::Draw(const HCamera& camera, const RendererUtility::RenderTextureData& viewportData)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        Vector<PickingUtils::PerHudInstanceData> instancedElements;

        _selectionMat->BindCamera(camera);

        if (!_selections.ClickedComponent && _selections.ClickedSceneObject) // A SceneObject has been selected, we need to highlight all sub elements
        {
            DrawInternal(camera, _selections.ClickedSceneObject, instancedElements, viewportData);
        }
        else if(_selections.ClickedComponent) // A single Component has been selected, easier
        {
            TypeID_Core type = (TypeID_Core)_selections.ClickedComponent->GetCoreType();

            switch (type)
            {
                case TID_CRenderable:
                {
                    SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(_selections.ClickedComponent);
                    if (renderable->GetActive() && gRendererUtility().DoFrustumCulling(camera, renderable))
                        DrawRenderable(renderable, viewportData);
                }
                break;

                default:
                break;
            }

            PickingUtils::FillPerInstanceHud(instancedElements, camera, 
                _selections.ClickedComponent->GetHandle(), PickingUtils::RenderType::Selection);
        }

        if (instancedElements.size() > 0)
        {
            rapi.SetRenderTarget(viewportData.RenderTex);
            rapi.PushMarker("[DRAW] HUD", Color(0.77f, 0.35f, 0.50f));

            _hudMat->BindCamera(camera, PickingUtils::RenderType::Selection);

            rapi.ClearViewport(FBT_DEPTH, Color::Black);

            rapi.SetVertexDeclaration(_instanceBuffer.PointVDecl);
            rapi.SetVertexBuffers(0, &_instanceBuffer.PointVB, 1);
            rapi.SetDrawOperation(DOT_POINT_LIST);

            UINT64 elementToDraw = static_cast<UINT64>(instancedElements.size());

            auto iterBegin = instancedElements.begin();
            auto iterRangeStart = iterBegin;
            auto iterRangeEnd = iterBegin + ((elementToDraw >= MAX_HUD_INSTANCED_BLOCK) ? MAX_HUD_INSTANCED_BLOCK : elementToDraw);

            do
            {
                UINT64 elementsDrawn = static_cast<UINT32>(iterRangeEnd - iterRangeStart);

                _hudMat->BindHud(iterRangeStart, iterRangeEnd);
                _hudMat->Bind();
                rapi.Draw(0, 1, static_cast<UINT32>(elementsDrawn));

                elementToDraw = elementToDraw - elementsDrawn;

                iterRangeStart = iterRangeEnd;
                iterRangeEnd = iterRangeStart + ((elementToDraw >= MAX_HUD_INSTANCED_BLOCK) ? MAX_HUD_INSTANCED_BLOCK : elementToDraw);
            } 
            while (elementToDraw > 0);

            rapi.PopMarker();
            rapi.SetRenderTarget(nullptr);
        }
    }

    void Selection::DrawInternal(const HCamera& camera, const SPtr<SceneObject>& sceneObject, Vector<PickingUtils::PerHudInstanceData>& instancedElements,
        const RendererUtility::RenderTextureData& viewportData)
    {
        for (const auto& component : sceneObject->GetComponents())
        {
            TypeID_Core type = (TypeID_Core)component->GetCoreType();

            switch (type)
            {
                case TID_CRenderable:
                {
                    HRenderable renderable = static_object_cast<CRenderable>(component);
                    if (renderable->GetActive() && gRendererUtility().DoFrustumCulling(camera, renderable))
                        DrawRenderable(renderable.GetInternalPtr(), viewportData);
                }
                break;

                default:
                break;
            }

            PickingUtils::FillPerInstanceHud(instancedElements, camera, component, PickingUtils::RenderType::Selection);
        }

        for (const auto& childSO : sceneObject->GetChildren())
            DrawInternal(camera, childSO.GetInternalPtr(), instancedElements, viewportData);
    }

    void Selection::DrawRenderable(const SPtr<CRenderable>& renderable, const RendererUtility::RenderTextureData& viewportData)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<Mesh> mesh = renderable->GetMesh();

        if (mesh)
        {
            _needOutline = true;

            rapi.SetRenderTarget(_renderablesOutput->RenderTex);
            rapi.PushMarker("[DRAW] Renderable", Color(0.56f, 0.75f, 0.32f));

            _selectionMat->BindRenderable(static_object_cast<CRenderable>(renderable->GetHandle()));
            _selectionMat->Bind();

            MeshProperties properties = mesh->GetProperties();
            UINT32 numMeshes = properties.GetNumSubMeshes();

            for (UINT32 i = 0; i < numMeshes; i++)
                gRendererUtility().Draw(mesh, properties.GetSubMesh(i), 1);

            rapi.PopMarker();
            rapi.SetRenderTarget(nullptr);
        }
    }

    void Selection::OutlineRenderables(const RendererUtility::RenderTextureData& viewportData)
    {
        if (!_needOutline)
            return;

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.PushMarker("[DRAW] Outline renderables", Color(0.67f, 0.25f, 0.50f));

        {
            rapi.PushMarker("[DRAW] Blur renderables", Color(0.35f, 0.57f, 0.62f));

            _selectionBlurMat->Execute(_renderablesOutput->ColorTex.GetInternalPtr(), _blurredRenderablesOutput->RenderTex, 0.6f, 24);

            rapi.PopMarker();
            rapi.SetRenderTarget(nullptr);
        }

        {
            rapi.PushMarker("[DRAW] Blit Outlines", Color(0.35f, 0.57f, 0.62f));

            {
                rapi.SetRenderTarget(_outlineRenderablesOutput->RenderTex);
                _selectionBlitMat->Execute(viewportData.RenderTex->GetColorTexture(0), _renderablesOutput->ColorTex.GetInternalPtr(), _blurredRenderablesOutput->ColorTex.GetInternalPtr());
                rapi.SetRenderTarget(nullptr);
            }


            {
                rapi.SetRenderTarget(viewportData.RenderTex);
                gRendererUtility().Blit(_outlineRenderablesOutput->ColorTex.GetInternalPtr());
                rapi.SetRenderTarget(nullptr);
            }

            rapi.PopMarker();
        }

        rapi.PopMarker();
    }

    void Selection::CreateRenderTexture(SPtr<RendererUtility::RenderTextureData>& renderTexture, UINT32 width, UINT32 height)
    {
        if (renderTexture && (renderTexture->Width != width || renderTexture->Height != height))
            ReleaseRenderTexture(renderTexture);

        if (!renderTexture)
        {
            renderTexture = te_shared_ptr_new<RendererUtility::RenderTextureData>();

            renderTexture->Width = width;
            renderTexture->Height = height;

            gRendererUtility().GenerateViewportRenderTexture(*renderTexture);
        }
    }

    void Selection::ReleaseRenderTexture(SPtr<RendererUtility::RenderTextureData>& renderTexture)
    {
        if (renderTexture->ColorTex.IsLoaded())
            renderTexture->ColorTex.Release();
        if (renderTexture->DepthStencilTex.IsLoaded())
            renderTexture->DepthStencilTex.Release();

        renderTexture = nullptr;
    }
}
