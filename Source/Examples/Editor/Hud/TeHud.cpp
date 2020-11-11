#include "TeHud.h"

#include "../TeEditor.h"
#include "RenderAPI/TeRenderAPI.h"
#include "RenderAPI/TeRenderTarget.h"
#include "Scene/TeSceneObject.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeLight.h"
#include "Components/TeCLight.h"
#include "RenderAPI/TeVertexDataDesc.h"

namespace te
{ 
    void Hud::Initialize()
    {
        _material = HudMat::Get();

        _pointVDesc = te_shared_ptr_new<VertexDataDesc>();
        _pointVDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

        _pointVDecl = VertexDeclaration::Create(_pointVDesc);

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

        Vector<HudMat::Element> matElements;
        GetHudMatElements(camera, root, matElements);

        if (matElements.size() > 0)
        {
            rapi.SetRenderTarget(camera->GetViewport()->GetTarget());
            rapi.ClearViewport(clearBuffers, Color::Black);

            _material->BindCamera(camera);

            rapi.SetVertexDeclaration(_pointVDecl);
            rapi.SetVertexBuffers(0, &_pointVB, 1);
            rapi.SetDrawOperation(DOT_POINT_LIST);

            UINT64 elementToDraw = static_cast<UINT64>(matElements.size());

            auto iterBegin = matElements.begin();
            auto iterRangeStart = iterBegin;
            auto iterRangeEnd = iterBegin + ((elementToDraw >= STANDARD_MAX_INSTANCED_BLOCK) ? STANDARD_MAX_INSTANCED_BLOCK : elementToDraw);

            do
            {
                UINT64 elementsDrawn = static_cast<UINT32>(iterRangeEnd - iterRangeStart);

                _material->BindHud(iterRangeStart, iterRangeEnd);
                _material->Bind();
                rapi.Draw(0, 1, static_cast<UINT32>(elementsDrawn));

                elementToDraw = elementToDraw - elementsDrawn;

                iterRangeStart = iterRangeEnd;
                iterRangeEnd = iterRangeStart + ((elementToDraw >= STANDARD_MAX_INSTANCED_BLOCK) ? STANDARD_MAX_INSTANCED_BLOCK : elementToDraw);
            } 
            while (elementToDraw > 0);

            rapi.SetRenderTarget(nullptr);
        } 
    }

    void Hud::GetHudMatElements(const HCamera& camera, const HSceneObject& sceneObject, Vector<HudMat::Element>& matElements)
    {
        HudMat::Element element;

        for (const auto& component : sceneObject->GetComponents())
        {
            TypeID_Core type = (TypeID_Core)component->GetCoreType();

            switch (type)
            {
                case TypeID_Core::TID_CCamera: 
                {
                    HCamera cameraElement = static_object_cast<CCamera>(component);
                    if (cameraElement->GetActive())
                    {
                        const Transform& tfrm = cameraElement->GetTransform();
                        element.WorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE);
                        element.ElemType = HudMat::Type::Camera;
                        element.ElemColor = Color::Black;

                        matElements.push_back(element);
                    }
                }
                break;

                case TypeID_Core::TID_CLight:
                {
                    HLight lightElement = static_object_cast<CLight>(component);
                    if (lightElement->GetActive())
                    {
                        const Transform& tfrm = lightElement->GetTransform();
                        element.WorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE);
                        element.ElemColor = lightElement->GetColor();

                        switch (lightElement->GetType())
                        {
                        case LightType::Directional:
                            element.ElemType = HudMat::Type::DirectionalLight;
                            break;
                        case LightType::Radial:
                            element.ElemType = HudMat::Type::RadialLight;
                            break;
                        case LightType::Spot:
                            element.ElemType = HudMat::Type::SpotLight;
                            break;
                        }

                        matElements.push_back(element);
                    }
                }
                break;
            }
        }

        for (const auto& childSO : sceneObject->GetChildren())
            GetHudMatElements(camera, childSO, matElements);
    }
}
