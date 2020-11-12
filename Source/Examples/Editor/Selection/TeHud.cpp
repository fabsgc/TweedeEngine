#include "TeHud.h"

#include "../TeEditor.h"
#include "../TeEditorUtils.h"
#include "RenderAPI/TeRenderAPI.h"
#include "RenderAPI/TeRenderTarget.h"
#include "Scene/TeSceneObject.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeLight.h"
#include "Components/TeCLight.h"
#include "TeHudPickingMat.h"

namespace te
{
    void Hud::Initialize()
    {
        _material = HudPickingMat::Get();

        SPtr<VertexDataDesc> pointVDesc = te_shared_ptr_new<VertexDataDesc>();
        pointVDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

        _pointVDecl = VertexDeclaration::Create(pointVDesc);

        VERTEX_BUFFER_DESC vbDesc;
        vbDesc.VertexSize = _pointVDecl->GetProperties().GetVertexSize(0);
        vbDesc.NumVerts = 1;
        vbDesc.Usage = GBU_DYNAMIC;

        _pointVB = VertexBuffer::Create(vbDesc);

        _pointData = (VertexBufferLayout *)_pointVB->Lock(0, sizeof(VertexBufferLayout), GBL_WRITE_ONLY_NO_OVERWRITE);

        if (_pointData)
            _pointData[0].Position = Vector3(0.0f, 0.0f, 0.0f);

        _pointVB->Unlock();
    }

    void Hud::Render(const HCamera& camera, const HSceneObject& root)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_DEPTH;

        const SPtr<RenderSettings>& settings = camera->GetRenderSettings();
        if (settings->OutputType != RenderOutputType::Final)
            return;

        Vector<EditorUtils::PerHudInstanceData> instancedElements;
        GetHudElements(camera, root, instancedElements);

        if (instancedElements.size() > 0)
        {
            rapi.SetRenderTarget(camera->GetViewport()->GetTarget());
            rapi.ClearViewport(clearBuffers, Color::Black);

            _material->BindCamera(camera, SelectionRenderType::Draw);

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

                _material->BindHud(iterRangeStart, iterRangeEnd);
                _material->Bind();
                rapi.Draw(0, 1, static_cast<UINT32>(elementsDrawn));

                elementToDraw = elementToDraw - elementsDrawn;

                iterRangeStart = iterRangeEnd;
                iterRangeEnd = iterRangeStart + ((elementToDraw >= MAX_HUD_INSTANCED_BLOCK) ? MAX_HUD_INSTANCED_BLOCK : elementToDraw);
            } 
            while (elementToDraw > 0);

            rapi.SetRenderTarget(nullptr);
        } 
    }

    void Hud::GetHudElements(const HCamera& camera, const HSceneObject& sceneObject, Vector<EditorUtils::PerHudInstanceData>& matElements)
    {
        EditorUtils::PerHudInstanceData element;

        for (const auto& component : sceneObject->GetComponents())
        {
            TypeID_Core type = (TypeID_Core)component->GetCoreType();

            switch (type)
            {
                case TypeID_Core::TID_CCamera: 
                {
                    HCamera cameraElement = static_object_cast<CCamera>(component);
                    if (cameraElement->GetActive() && EditorUtils::DoFrustumCulling(camera, cameraElement))
                    {
                        const Transform& tfrm = cameraElement->GetTransform();
                        element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE).Transpose();
                        element.Type = static_cast<float>(HudType::Camera);
                        element.Color = Color::Black.GetAsVector4();

                        matElements.push_back(element);
                    }
                }
                break;

                case TypeID_Core::TID_CLight:
                {
                    HLight lightElement = static_object_cast<CLight>(component);
                    if (lightElement->GetActive() && EditorUtils::DoFrustumCulling(camera, lightElement))
                    {
                        const Transform& tfrm = lightElement->GetTransform();
                        element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE).Transpose();
                        element.Color = lightElement->GetColor().GetAsVector4();

                        switch (lightElement->GetType())
                        {
                        case LightType::Directional:
                            element.Type = static_cast<float>(HudType::DirectionalLight);
                            break;
                        case LightType::Radial:
                            element.Type = static_cast<float>(HudType::RadialLight);
                            break;
                        case LightType::Spot:
                            element.Type = static_cast<float>(HudType::SpotLight);
                            break;
                        }

                        matElements.push_back(element);
                    }
                }
                break;
            }
        }

        for (const auto& childSO : sceneObject->GetChildren())
            GetHudElements(camera, childSO, matElements);
    }
}
