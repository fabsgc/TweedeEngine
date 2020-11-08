#include "TeGpuPicking.h"

#include "TeEditor.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Resources/TeBuiltinResources.h"
#include "Renderer/TeRenderable.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCCamera.h"
#include "Scene/TeTransform.h"

namespace te
{
    void GpuPicking::Initialize()
    { 
        _shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::GpuPicking);
        _perCameraParamBuffer = _perCameraParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();
    }

    void GpuPicking::ComputePicking(const HCamera& camera, const RenderParam& param, const HSceneObject& root)
    { 
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();
        
        // Check if texture is up to date
        CheckRenderTexture(param.Width, param.Height);

        // Update per camera gpu buffer
        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perCameraParamDef.gMatViewProj.Set(_perCameraParamBuffer, viewProjMatrix.Transpose());

        // Configurer output
        rapi.SetRenderTarget(_renderData.RenderTex);
        rapi.ClearViewport(clearBuffers, Color::Blue);

        // TODO draw
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
        Color color = GetColorAt(x, y);
        auto& iter = _colorToGameObject.find(color.GetAsRGBA());

        if (iter != _colorToGameObject.end())
            return iter->second;

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
            UINT64 instanceId = component->GetInstanceId();
            Color color = component->GetColor();
            RGBA rgbaColor = color.GetAsRGBA();

            if (_colorToGameObject.find(rgbaColor) == _colorToGameObject.end())
                _colorToGameObject[rgbaColor] = component.GetInternalPtr();

            TE_PRINT(ToString(color.r) + "/" + ToString(color.b) + "/" + ToString(color.b))

            switch (type)
            {
                case TypeID_Core::TID_CRenderable: 
                {
                    HRenderable renderable = static_object_cast<CRenderable>(component);
                    if (DoFrustumCulling(camera, renderable))
                        DrawRenderable(renderable);
                } 
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
        // Update per object gpu buffer
        _perObjectParamDef.gMatWorld.Set(_perObjectParamBuffer, renderable->GetMatrix().Transpose());
        _perObjectParamDef.gColor.Set(_perObjectParamBuffer, renderable->GetColor().GetAsVector4());

        // Render
    }

    void GpuPicking::DrawLight(const HLight& light)
    { 
        // TODO
    }

    void GpuPicking::DrawCamera(const HCamera& camera)
    { 
        // TODO
    }
}
