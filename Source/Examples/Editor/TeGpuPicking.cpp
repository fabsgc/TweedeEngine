#include "TeGpuPicking.h"

#include "TeEditor.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCCamera.h"
#include "Scene/TeTransform.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    void GpuPicking::Initialize()
    {
        _material = GpuPickingMat::Get();
    }

    void GpuPicking::ComputePicking(const HCamera& camera, const RenderParam& param, const HSceneObject& root)
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
    }

    Color GpuPicking::GetColorAt(UINT32 x, UINT32 y)
    { 
        SPtr<Texture> pickingTexture = (static_cast<RenderTexture&>(*(_renderData.RenderTex))).GetColorTexture(0);
        SPtr<PixelData> pixelData = pickingTexture->GetProperties().AllocBuffer(0, 0);
        pickingTexture->ReadData(*pixelData);

        return pixelData->GetColorAt(x, y);
    }

    SPtr<GameObject> GpuPicking::GetGameObjectAt(UINT32 x, UINT32 y)
    {
        Color pickedColor = GetColorAt(x, y);
        RGBA pickedColorRGBA = pickedColor.GetAsRGBA();

        const auto& iterFind = _colorToGameObject.find(pickedColorRGBA);
        if (iterFind != _colorToGameObject.end())
            return iterFind->second;

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
                Vector4 objColorObjV4 = iterObject.second->GetColor().GetAsVector4();
                Vector3 objColorObjV3 = Vector3(objColorObjV4.x, objColorObjV4.y, objColorObjV4.z);

                if(fabs(objColorObjV3.SquaredDistance(pickedColorV3)) < 1e-3)
                    return iterObject.second;
            }
        }

        return nullptr;
    }

    bool GpuPicking::CheckRenderTexture(UINT32 width, UINT32 height)
    {
        if (width == _renderData.Width && height == _renderData.Height)
            return false;

        _renderData.Width = width;
        _renderData.Height = height;

        EditorUtils::GenerateViewportRenderTexture(_renderData);

        return true;
    }

    bool GpuPicking::DoFrustumCulling(const HCamera& camera, const HRenderable& renderable)
    {
        ConvexVolume worldFrustum = camera->GetWorldFrustum();
        const Vector3& worldCameraPosition = camera->GetTransform().GetPosition();
        float baseCullDistance = camera->GetRenderSettings()->CullDistance;

        Bounds boundaries = renderable->GetBounds();
        const Sphere& boundingSphere = boundaries.GetSphere();
        const Vector3& worldRenderablePosition = boundingSphere.GetCenter();

        float distanceToCameraSq = worldCameraPosition.SquaredDistance(worldRenderablePosition);
        float correctedCullDistance = renderable->GetCullDistanceFactor() * baseCullDistance;
        float maxDistanceToCamera = correctedCullDistance + boundingSphere.GetRadius();

        if (distanceToCameraSq > maxDistanceToCamera* maxDistanceToCamera)
            return false;
        
        if (worldFrustum.Intersects(boundingSphere))
        {
            const AABox& boundingBox = boundaries.GetBox();
            if (worldFrustum.Intersects(boundingBox))
                return true;
        }

        return false;
    }

    bool GpuPicking::DoFrustumCulling(const HCamera& camera, const HLight& light)
    {
        // TODO

        return false;
    }

    bool GpuPicking::DoFrustumCulling(const HCamera& camera, const HCamera& sceneCamera)
    { 
        // TODO

        return false;
    }

    void GpuPicking::Draw(const HCamera& camera, const HSceneObject& sceneObject)
    { 
        for (const auto& component : sceneObject->GetComponents())
        {
            UINT32 type = component->GetCoreType();
            Color color = component->GetColor();
            RGBA rgbaColor = color.GetAsRGBA();

            if (_colorToGameObject.find(rgbaColor) == _colorToGameObject.end())
                _colorToGameObject[rgbaColor] = component.GetInternalPtr();

            TE_PRINT(ToString(color.r) + "/" + ToString(color.g) + "/" + ToString(color.b));

            switch (type)
            {
                case TypeID_Core::TID_CRenderable: 
                {
                    HRenderable renderable = static_object_cast<CRenderable>(component);
                    if (DoFrustumCulling(camera, renderable))
                        DrawRenderable(renderable);
                }
                break;

                case TypeID_Core::TID_CLight:
                {
                    HLight light = static_object_cast<CLight>(component);
                    if (DoFrustumCulling(camera, light))
                        DrawLight(light);
                }
                break;

                case TypeID_Core::TID_CCamera:
                {
                    HCamera renderableCamera = static_object_cast<CCamera>(component);
                    if (DoFrustumCulling(camera, renderableCamera))
                        DrawCamera(renderableCamera);
                }
                break;

                default:
                    break;
            }
        }

        for (const auto& childSO : sceneObject->GetChildren())
        {
            Draw(camera, childSO);
        }
    }

    void GpuPicking::DrawRenderable(const HRenderable& renderable)
    {
        SPtr<Mesh> mesh = renderable->GetMesh();

        if (mesh)
        {
            _material->BindRenderable(renderable);
            _material->Bind();

            MeshProperties properties = mesh->GetProperties();
            UINT32 numMeshes = properties.GetNumSubMeshes();

            for (UINT32 i = 0; i < numMeshes; i++)
            {
                gRendererUtility().Draw(mesh, properties.GetSubMesh(i), 1);
            }
        }        
    }

    void GpuPicking::DrawLight(const HLight& light)
    {
        _material->BindLight(light);
        _material->Bind();

        // Render
    }

    void GpuPicking::DrawCamera(const HCamera& camera)
    {
        _material->BindRenderableCamera(camera);
        _material->Bind();

        // Render
    }
}
