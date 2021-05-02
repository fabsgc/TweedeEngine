#include "TePicking.h"

#include "../TeEditor.h"
#include "Scene/TeTransform.h"
#include "Scene/TeSceneObject.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCCamera.h"
#include "Renderer/TeRendererUtility.h"
#include "TePickingMat.h"
#include "TeHudPickingMat.h"
#include "Utility/TeTime.h"
#include "Mesh/TeMesh.h"

namespace te
{
    Picking::Picking()
        : _material(nullptr)
        , _hudMaterial(nullptr)
    { }

    void Picking::Initialize()
    {
        _material = PickingMat::Get();
        _hudMaterial = HudPickingMat::Get();

        SelectionUtils::CreateHudInstanceBuffer(_instanceBuffer);
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
        Vector3 blackColorV3 = Vector3::ZERO;

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
                        return object;
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

    void Picking::DrawInternal(const HCamera& camera, const HSceneObject& sceneObject, Vector<SelectionUtils::PerHudInstanceData>& instancedElements)
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

                default:
                break;
            }

            SelectionUtils::FillPerInstanceHud(instancedElements, camera, component, SelectionUtils::RenderType::Picking);
        }

        for (const auto& childSO : sceneObject->GetChildren())
            DrawInternal(camera, childSO, instancedElements);
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
