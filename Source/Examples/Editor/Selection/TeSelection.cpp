#include "TeSelection.h"

#include "TeSelectionMat.h"
#include "TeHudSelectionMat.h"
#include "Components/TeCCamera.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    Selection::Selection()
        : _selections(gEditor().GetSelectionData())
    { }

    void Selection::Initialize()
    {
        _material = SelectionMat::Get();
        _hudMaterial = HudSelectionMat::Get();

        {
            SPtr<VertexDataDesc> pointVDesc = te_shared_ptr_new<VertexDataDesc>();
            pointVDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

            _pointVDecl = VertexDeclaration::Create(pointVDesc);

            VERTEX_BUFFER_DESC vbDesc;
            vbDesc.VertexSize = _pointVDecl->GetProperties().GetVertexSize(0);
            vbDesc.NumVerts = 1;
            vbDesc.Usage = GBU_DYNAMIC;

            _pointVB = VertexBuffer::Create(vbDesc);

            _pointData = (SelectionUtils::VertexBufferLayout *)_pointVB->Lock(
                0, sizeof(SelectionUtils::VertexBufferLayout), GBL_WRITE_ONLY_NO_OVERWRITE);

            if (_pointData)
                _pointData[0].Position = Vector3(0.0f, 0.0f, 0.0f);

            _pointVB->Unlock();
        }
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

                case TypeID_Core::TID_CLight:
                {
                    SPtr<CLight> light = std::static_pointer_cast<CLight>(_selections.ClickedComponent);
                    if (light->GetActive() && EditorUtils::DoFrustumCulling(camera, light))
                    {
                        SelectionUtils::PerHudInstanceData element;
                        const Transform& tfrm = light->GetTransform();
                        element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE).Transpose();
                        element.Color = light->GetColor().GetAsVector4();

                        switch (light->GetType())
                        {
                        case LightType::Directional:
                            element.Type = static_cast<float>(SelectionUtils::HudType::DirectionalLight);
                            break;
                        case LightType::Radial:
                            element.Type = static_cast<float>(SelectionUtils::HudType::RadialLight);
                            break;
                        case LightType::Spot:
                            element.Type = static_cast<float>(SelectionUtils::HudType::SpotLight);
                            break;
                        }

                        instancedElements.push_back(element);
                    }
                }
                break;

                case TypeID_Core::TID_CCamera:
                {
                    SPtr<CCamera> cameraElement = std::static_pointer_cast<CCamera>(_selections.ClickedComponent);
                    if (cameraElement->GetActive() && EditorUtils::DoFrustumCulling(camera, cameraElement))
                    {
                        SelectionUtils::PerHudInstanceData element;
                        const Transform& tfrm = cameraElement->GetTransform();
                        element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE).Transpose();
                        element.Type = static_cast<float>(SelectionUtils::HudType::Camera);
                        element.Color = Color::Black.GetAsVector4();

                        instancedElements.push_back(element);
                    }
                }
                break;

                default:
                break;
            }
        }

        if (instancedElements.size() > 0)
        {
            _hudMaterial->BindCamera(camera, SelectionUtils::RenderType::Selection);

            UINT32 clearBuffers = FBT_DEPTH;
            rapi.ClearViewport(clearBuffers, Color::Black);

            rapi.SetVertexDeclaration(_pointVDecl);
            rapi.SetVertexBuffers(0, &_pointVB, 1);
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

    void Selection::DrawInternal(const HCamera& camera, const SPtr<SceneObject>& sceneObject, Vector<SelectionUtils::PerHudInstanceData>& matElements)
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

                case TypeID_Core::TID_CLight:
                {
                    HLight light = static_object_cast<CLight>(component);
                    if (light->GetActive() && EditorUtils::DoFrustumCulling(camera, light))
                    {
                        SelectionUtils::PerHudInstanceData element;
                        const Transform& tfrm = light->GetTransform();
                        element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE).Transpose();
                        element.Color = light->GetColor().GetAsVector4();

                        switch (light->GetType())
                        {
                        case LightType::Directional:
                            element.Type = static_cast<float>(SelectionUtils::HudType::DirectionalLight);
                            break;
                        case LightType::Radial:
                            element.Type = static_cast<float>(SelectionUtils::HudType::RadialLight);
                            break;
                        case LightType::Spot:
                            element.Type = static_cast<float>(SelectionUtils::HudType::SpotLight);
                            break;
                        }

                        matElements.push_back(element);
                    }
                }
                break;

                case TypeID_Core::TID_CCamera:
                {
                    HCamera cameraElement = static_object_cast<CCamera>(component);
                    if (cameraElement->GetActive() && EditorUtils::DoFrustumCulling(camera, cameraElement))
                    {
                        SelectionUtils::PerHudInstanceData element;
                        const Transform& tfrm = cameraElement->GetTransform();
                        element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE).Transpose();
                        element.Type = static_cast<float>(SelectionUtils::HudType::Camera);
                        element.Color = Color::Black.GetAsVector4();

                        matElements.push_back(element);
                    }
                }
                break;

                default:
                break;
            }
        }

        for (const auto& childSO : sceneObject->GetChildren())
            DrawInternal(camera, childSO.GetInternalPtr(), matElements);
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
}
