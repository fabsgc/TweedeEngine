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
                    Vector4 objColorObjV4 = object->GetColor().GetAsVector4();
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
        Vector<HCamera> cameras;
        Vector<HLight> lights;

        DrawInternal(camera, sceneObject, lights, cameras);

        if (lights.size() > 0)
            DrawLights(lights);

        if (cameras.size() > 0)
            DrawCameras(cameras);
    }

    void Picking::DrawInternal(const HCamera& camera, const HSceneObject& sceneObject, Vector<HLight>& lights, Vector<HCamera>& cameras)
    {
        float now = gTime().GetTime();

        for (const auto& component : sceneObject->GetComponents())
        {
            UINT32 type = component->GetCoreType();
            Color color = component->GetColor();
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
                        lights.push_back(light);
                }
                break;

                case TypeID_Core::TID_CCamera:
                {
                    HCamera renderableCamera = static_object_cast<CCamera>(component);
                    if (renderableCamera->GetActive() && EditorUtils::DoFrustumCulling(camera, renderableCamera))
                        cameras.push_back(renderableCamera);
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

    void Picking::DrawLights(const Vector<HLight>& light)
    {
        // TODO
    }

    void Picking::DrawCameras(const Vector<HCamera>& light)
    {
        // TODO
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
