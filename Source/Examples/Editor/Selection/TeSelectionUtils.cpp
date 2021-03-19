#include "TeSelectionUtils.h"

namespace te
{
    namespace SelectionUtils
    {
        void CreateHudInstanceBuffer(HudInstanceBuffer& buffer)
        {
            buffer.PointVDesc = te_shared_ptr_new<VertexDataDesc>();
            buffer.PointVDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

            buffer.PointVDecl = VertexDeclaration::Create(buffer.PointVDesc);

            VERTEX_BUFFER_DESC vbDesc;
            vbDesc.VertexSize = buffer.PointVDecl->GetProperties().GetVertexSize(0);
            vbDesc.NumVerts = 1;
            vbDesc.Usage = GBU_DYNAMIC;

            buffer.PointVB = VertexBuffer::Create(vbDesc);

            buffer.PointData = (VertexBufferLayout *)buffer.PointVB->Lock(
                0, sizeof(VertexBufferLayout), GBL_WRITE_ONLY_NO_OVERWRITE);

            if (buffer.PointData)
                buffer.PointData[0].Position = Vector3(0.0f, 0.0f, 0.0f);

            buffer.PointVB->Unlock();
        }

        void FillPerInstanceHud(Vector<PerHudInstanceData>& instancedElements, const HCamera& camera, const HComponent& component, RenderType renderType)
        {
            TypeID_Core type = (TypeID_Core)component->GetCoreType();

            switch (type)
            {
            case TypeID_Core::TID_CLight:
            {
                HLight light = static_object_cast<CLight>(component);
                if (light->GetActive() && EditorUtils::DoFrustumCulling(camera, light))
                {
                    PerHudInstanceData element;
                    const Transform& tfrm = light->GetTransform();
                    element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE);

                    if(renderType == RenderType::Selection || renderType == RenderType::Draw)
                        element.Color = light->GetColor().GetAsVector4();
                    else if (renderType == RenderType::Picking)
                        element.Color = light->GetGameObjectColor().GetAsVector4();

                    switch (light->GetType())
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
                    default:
                        break;
                    }

                    instancedElements.push_back(element);
                }
            }
            break;

            case TypeID_Core::TID_CCamera:
            {
                HCamera cameraElement = static_object_cast<CCamera>(component);
                if (cameraElement->GetActive() && EditorUtils::DoFrustumCulling(camera, cameraElement))
                {
                    PerHudInstanceData element;
                    const Transform& tfrm = cameraElement->GetTransform();
                    element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE);
                    element.Type = static_cast<float>(HudType::Camera);

                    if (renderType == RenderType::Selection || renderType == RenderType::Draw)
                        element.Color = Color::Black.GetAsVector4();
                    else if (renderType == RenderType::Picking)
                        element.Color = cameraElement->GetGameObjectColor().GetAsVector4();

                    instancedElements.push_back(element);
                }
            }
            break;

            case TypeID_Core::TID_CAudioListener:
            {
                HAudioListener audio = static_object_cast<CAudioListener>(component);
                if (audio->GetActive() && EditorUtils::DoFrustumCulling(camera, audio))
                {
                    PerHudInstanceData element;
                    const Transform& tfrm = audio->GetTransform();
                    element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE);
                    element.Type = static_cast<float>(HudType::AudioListener);

                    if (renderType == RenderType::Selection || renderType == RenderType::Draw)
                        element.Color = Color::White.GetAsVector4();
                    else if (renderType == RenderType::Picking)
                        element.Color = audio->GetGameObjectColor().GetAsVector4();

                    instancedElements.push_back(element);
                }
            }
            break;

            case TypeID_Core::TID_CAudioSource:
            {
                HAudioSource audio = static_object_cast<CAudioSource>(component);
                if (audio->GetActive() && EditorUtils::DoFrustumCulling(camera, audio))
                {
                    PerHudInstanceData element;
                    const Transform& tfrm = audio->GetTransform();
                    element.MatWorldNoScale = Matrix4::TRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE);
                    element.Type = static_cast<float>(HudType::AudioSource);

                    if (renderType == RenderType::Selection || renderType == RenderType::Draw)
                        element.Color = Color::White.GetAsVector4();
                    else if (renderType == RenderType::Picking)
                        element.Color = audio->GetGameObjectColor().GetAsVector4();

                    instancedElements.push_back(element);
                }
            }
            break;

            default:
                break;
            }
        }
    }
}
