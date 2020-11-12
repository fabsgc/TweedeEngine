#include "TePicking.h"

#include "../TeEditor.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCCamera.h"
#include "Scene/TeTransform.h"
#include "Renderer/TeRendererUtility.h"
#include "Utility/TeTime.h"
#include "TePickingMat.h"
#include "TeHudPickingMat.h"

namespace te
{
    void Picking::Initialize()
    {
        _material = PickingMat::Get();
        _hudMaterial = HudPickingMat::Get();

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

    void Picking::ComputePicking(const HCamera& camera, const RenderParam& param, const HSceneObject& root)
    { 
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;
        
        // Check if texture is up to date
        CheckRenderTexture(param.Width, param.Height);

        // Bind camera param buffer
        _material->BindCamera(camera);

        // Configure output
        rapi.SetRenderTarget(_renderData.RenderTex);
        rapi.ClearViewport(clearBuffers, Color::Black);

        Draw(camera, root);

        rapi.SetRenderTarget(nullptr);

        CleanGameObjectsList();
    }

    Color Picking::GetColorAt(UINT32 x, UINT32 y)
    { 
        SPtr<Texture> pickingTexture = _renderData.RenderTex->GetColorTexture(0);
        SPtr<PixelData> pixelData = pickingTexture->GetProperties().AllocBuffer(0, 0);
        pickingTexture->ReadData(*pixelData);

        return pixelData->GetColorAt(x, y);
    }

    SPtr<GameObject> Picking::GetGameObjectAt(UINT32 x, UINT32 y)
    {
        Color pickedColor = GetColorAt(x, y);
        RGBA pickedColorRGBA = pickedColor.GetAsRGBA();

        const auto& iterFind = _colorToGameObject.find(pickedColorRGBA);
        if (iterFind != _colorToGameObject.end())
            return iterFind->second.Object.lock();

        // Due to float precision, sometimes, an object can't be found
        // If picked color is different from black, we try to find the closest color in objects
        // If the distance between the picked color and this object color is small enough we
        // can consider that this object is the current picked object
        Vector4 pickedColorV4 = pickedColor.GetAsVector4();
        Vector3 pickedColorV3 = Vector3(pickedColorV4.x, pickedColorV4.y, pickedColorV4.z);
        Vector3 blackColorV3;

        if (fabs(blackColorV3.SquaredDistance(pickedColorV3)) > 1e-3)
        {
            for (auto& iterObject : _colorToGameObject)
            {
                SPtr<GameObject> object = iterObject.second.Object.lock();

                // GameObject could have been deleted
                if (object)
                {
                    Vector4 objColorObjV4 = object->GetGameObjectColor().GetAsVector4();
                    Vector3 objColorObjV3 = Vector3(objColorObjV4.x, objColorObjV4.y, objColorObjV4.z);

                    if (fabs(objColorObjV3.SquaredDistance(pickedColorV3)) < 1e-3)
                    {
                        SPtr<GameObject> object = iterObject.second.Object.lock();
                        return object;
                    }
                }
            }
        }

        return nullptr;
    }

    bool Picking::CheckRenderTexture(UINT32 width, UINT32 height)
    {
        if (width == _renderData.Width && height == _renderData.Height)
            return false;

        _renderData.Width = width;
        _renderData.Height = height;

        EditorUtils::GenerateViewportRenderTexture(_renderData);

        return true;
    }

    void Picking::Draw(const HCamera& camera, const HSceneObject& sceneObject)
    { 
        Vector<SelectionUtils::PerHudInstanceData> instancedElements;

        DrawInternal(camera, sceneObject, instancedElements);

        if (instancedElements.size() > 0)
        {
            _hudMaterial->BindCamera(camera, SelectionUtils::RenderType::Picking);

            RenderAPI& rapi = RenderAPI::Instance();
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

    void Picking::DrawInternal(const HCamera& camera, const HSceneObject& sceneObject, Vector<SelectionUtils::PerHudInstanceData>& matElements)
    {
        float now = gTime().GetTime();

        for (const auto& component : sceneObject->GetComponents())
        {
            UINT32 type = component->GetCoreType();
            Color color = component->GetGameObjectColor();
            RGBA rgbaColor = color.GetAsRGBA();

            if (_colorToGameObject.find(rgbaColor) == _colorToGameObject.end())
                _colorToGameObject[rgbaColor] = GameObjectInfo(component.GetInternalPtr());

            _colorToGameObject[rgbaColor].LastUse = now;

            switch (type)
            {
                case TypeID_Core::TID_CRenderable:
                {
                    HRenderable renderable = static_object_cast<CRenderable>(component);
                    if (renderable->GetActive() && EditorUtils::DoFrustumCulling(camera, renderable))
                        DrawRenderable(renderable);
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
                        element.Color = light->GetGameObjectColor().GetAsVector4();

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
                        element.Color = cameraElement->GetGameObjectColor().GetAsVector4();

                        matElements.push_back(element);
                    }
                }
                break;

                default:
                break;
            }
        }

        for (const auto& childSO : sceneObject->GetChildren())
            Draw(camera, childSO);
    }

    void Picking::DrawRenderable(const HRenderable& renderable)
    {
        SPtr<Mesh> mesh = renderable->GetMesh();

        if (mesh)
        {
            _material->BindRenderable(renderable);
            _material->Bind();

            MeshProperties properties = mesh->GetProperties();
            UINT32 numMeshes = properties.GetNumSubMeshes();

            for (UINT32 i = 0; i < numMeshes; i++)
                gRendererUtility().Draw(mesh, properties.GetSubMesh(i), 1);
        }
    }

    void Picking::CleanGameObjectsList()
    {
        float now = gTime().GetTime();
        for (auto it = _colorToGameObject.cbegin(); it != _colorToGameObject.cend(); )
        {
            if (it->second.LastUse < now - 10.0f)
                it = _colorToGameObject.erase(it);
            else
                ++it;
        }
    }
}
