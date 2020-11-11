#include "TeSelection.h"

#include "TeGpuPickingMat.h"
#include "TeHudPickingMat.h"
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
        _material = GpuPickingMat::Get();
        _hudMaterial = HudPickingMat::Get();
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
        Vector<SPtr<CCamera>> cameras;
        Vector<SPtr<CLight>> lights;

        _material->BindCamera(camera, GpuPickingMat::RenderType::Selection);

        if (!_selections.ClickedComponent && _selections.ClickedSceneObject) // A SceneObject has been selected, we need to highlight all sub elements
        {
            DrawInternal(camera, _selections.ClickedSceneObject, lights, cameras);
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
                        lights.push_back(light);
                }
                break;

                case TypeID_Core::TID_CCamera:
                {
                    SPtr<CCamera> renderableCamera = std::static_pointer_cast<CCamera>(_selections.ClickedComponent);
                    if (renderableCamera->GetActive() && EditorUtils::DoFrustumCulling(camera, renderableCamera))
                        cameras.push_back(renderableCamera);
                }
                break;
            }
        }

        // Bind camera param buffer
        _hudMaterial->BindCamera(camera, HudPickingMat::RenderType::Selection);

        if (lights.size() > 0)
            DrawLights(lights);

        if (cameras.size() > 0)
            DrawCameras(cameras);
    }

    void Selection::DrawInternal(const HCamera& camera, const SPtr<SceneObject>& sceneObject, Vector<SPtr<CLight>>& lights, Vector<SPtr<CCamera>>& cameras)
    {

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

    void Selection::DrawLights(const Vector<SPtr<CLight>>& light)
    {
        // TODO
    }

    void Selection::DrawCameras(const Vector<SPtr<CCamera>>& camera)
    {
        // TODO
    }
}
