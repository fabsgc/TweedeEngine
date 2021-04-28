#include "TeSelection.h"

#include "TeSelectionMat.h"
#include "TeHudSelectionMat.h"
#include "Components/TeCCamera.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCRigidBody.h"
#include "Renderer/TeRendererUtility.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    Selection::Selection()
        : _selections(gEditor().GetSelectionData())
    { }

    void Selection::Initialize()
    {
        _material = SelectionMat::Get();
        _hudMaterial = HudSelectionMat::Get();

        SelectionUtils::CreateHudInstanceBuffer(_instanceBuffer);
    }

    void Selection::Render(const HCamera& camera, const EditorUtils::RenderWindowData& viewportData)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_DEPTH;

        rapi.SetRenderTarget(viewportData.RenderTex);
        rapi.ClearViewport(clearBuffers, Color::Black);
        Draw(camera, viewportData);
        rapi.SetRenderTarget(nullptr);
    }

    void Selection::Draw(const HCamera& camera, const EditorUtils::RenderWindowData& viewportData)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        Vector<SelectionUtils::PerHudInstanceData> instancedElements;

        _material->BindCamera(camera);

        if (!_selections.ClickedComponent && _selections.ClickedSceneObject) // A SceneObject has been selected, we need to highlight all sub elements
        {
            DrawInternal(camera, _selections.ClickedSceneObject, instancedElements);
        }
        else if(_selections.ClickedComponent) // A single Component has been selected, easier
        {
            TypeID_Core type = (TypeID_Core)_selections.ClickedComponent->GetCoreType();

            switch (type)
            {
                case TID_CRenderable:
                {
                    SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(_selections.ClickedComponent);
                    if (renderable->GetActive() && EditorUtils::DoFrustumCulling(camera, renderable))
                        DrawRenderable(renderable);
                }
                break;

                case TID_CRigidBody:
                {
                    SPtr<CRigidBody> rigidBody = std::static_pointer_cast<CRigidBody>(_selections.ClickedComponent);
                    if (rigidBody && EditorUtils::DoFrustumCulling(camera, rigidBody))
                        DrawRigidBody(rigidBody);
                }
                break;

                case TID_CSoftBody:
                {
                    SPtr<CSoftBody> softBody = std::static_pointer_cast<CSoftBody>(_selections.ClickedComponent);
                    if (softBody && EditorUtils::DoFrustumCulling(camera, softBody))
                        DrawSoftBody(softBody);
                }
                break;

                default:
                break;
            }

            SelectionUtils::FillPerInstanceHud(instancedElements, camera, 
                _selections.ClickedComponent->GetHandle(), SelectionUtils::RenderType::Selection);
        }

        if (instancedElements.size() > 0)
        {
            _hudMaterial->BindCamera(camera, SelectionUtils::RenderType::Selection);

            UINT32 clearBuffers = FBT_DEPTH;
            rapi.ClearViewport(clearBuffers, Color::Black);

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

                _hudMaterial->BindHud(iterRangeStart, iterRangeEnd);
                _hudMaterial->Bind();
                rapi.Draw(0, 1, static_cast<UINT32>(elementsDrawn));

                elementToDraw = elementToDraw - elementsDrawn;

                iterRangeStart = iterRangeEnd;
                iterRangeEnd = iterRangeStart + ((elementToDraw >= MAX_HUD_INSTANCED_BLOCK) ? MAX_HUD_INSTANCED_BLOCK : elementToDraw);
            } 
            while (elementToDraw > 0);
        }
    }

    void Selection::DrawInternal(const HCamera& camera, const SPtr<SceneObject>& sceneObject, Vector<SelectionUtils::PerHudInstanceData>& instancedElements)
    {
        for (const auto& component : sceneObject->GetComponents())
        {
            TypeID_Core type = (TypeID_Core)component->GetCoreType();

            switch (type)
            {
                case TID_CRenderable:
                {
                    HRenderable renderable = static_object_cast<CRenderable>(component);
                    if (renderable->GetActive() && EditorUtils::DoFrustumCulling(camera, renderable))
                        DrawRenderable(renderable.GetInternalPtr());
                }
                break;

                case TID_CRigidBody:
                {
                    HRigidBody rigidBody = static_object_cast<CRigidBody>(component);
                    if (rigidBody && EditorUtils::DoFrustumCulling(camera, rigidBody))
                        DrawRigidBody(rigidBody.GetInternalPtr());
                }
                break;

                case TID_CSoftBody:
                {
                    HSoftBody softBody = static_object_cast<CSoftBody>(component);
                    if (softBody && EditorUtils::DoFrustumCulling(camera, softBody))
                        DrawSoftBody(softBody.GetInternalPtr());
                }
                break;

                default:
                break;
            }

            SelectionUtils::FillPerInstanceHud(instancedElements, camera, component, SelectionUtils::RenderType::Selection);
        }

        for (const auto& childSO : sceneObject->GetChildren())
            DrawInternal(camera, childSO.GetInternalPtr(), instancedElements);
    }

    void Selection::DrawRenderable(const SPtr<CRenderable>& renderable)
    {
        SPtr<Mesh> mesh = renderable->GetMesh();

        if (mesh)
        {
            _material->BindRenderable(static_object_cast<CRenderable>(renderable->GetHandle()));
            _material->Bind();

            MeshProperties properties = mesh->GetProperties();
            UINT32 numMeshes = properties.GetNumSubMeshes();

            for (UINT32 i = 0; i < numMeshes; i++)
                gRendererUtility().Draw(mesh, properties.GetSubMesh(i), 1);
        }
    }

    void Selection::DrawRigidBody(const SPtr<CRigidBody>& rigidBody)
    {
        // TODO
    }

    void Selection::DrawSoftBody(const SPtr<CSoftBody>& softBody)
    {
        // TODO
    }
}
