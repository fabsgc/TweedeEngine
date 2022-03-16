#include "TeMaterialsPreview.h"

#include "Mesh/TeMesh.h"
#include "Image/TeTexture.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeSkybox.h"
#include "Renderer/TeLight.h"
#include "Renderer/TeRenderable.h"
#include "Material/TeMaterial.h"
#include "Resources/TeResourceManager.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Manager/TeRendererManager.h"
#include "TeCoreApplication.h"

namespace te
{
    const UINT32 MaterialsPreview::PreviewSize = 360;
    const Color  MaterialsPreview::BackgroundColor = Color(0.3f, 0.36f, 0.48f, 1.0f);

    MaterialsPreview::Preview::Preview()
    {
        MatPreview = te_shared_ptr_new<RendererUtility::RenderTextureData>();

        MatPreview->Width = MaterialsPreview::PreviewSize;
        MatPreview->Height = MaterialsPreview::PreviewSize;

        gRendererUtility().GenerateViewportRenderTexture(*MatPreview);
    }

    MaterialsPreview::Preview::~Preview()
    {
        if (MatPreview->ColorTex.IsLoaded())
            MatPreview->ColorTex.Release();
        if (MatPreview->DepthStencilTex.IsLoaded())
            MatPreview->DepthStencilTex.Release();
    }

    MaterialsPreview::MaterialsPreview()
        : _meshPreviewType(MeshPreviewType::Sphere)
    { 
        InitializeTextures();
        InitializeCamera();
        InitializeSkybox();
        InitializeLight();
        InitializeRenderable();
        InitializeRenderer();

        _perFrameData = te_shared_ptr_new<FrameData>();
    }

    MaterialsPreview::~MaterialsPreview()
    {
        _renderer->NotifyCamerasCleared();
        _renderer->NotifyLightsCleared();
        _renderer->NotifySkyboxCleared();
        _renderer->NotifyRenderablesCleared();
    }

    const RendererUtility::RenderTextureData& MaterialsPreview::GetPreview(const WPtr<Material>& material)
    {
        auto it = _previews.find(material);
        if (it != _previews.end())
        {
            if (it->second->IsDirty)
                DrawMaterial(material, *it->second);

            return *(it->second->MatPreview);
        }
        else
        {
            _previews[material] = te_unique_ptr_new<Preview>();
            DrawMaterial(material, *_previews[material]);

            return *(_previews[material]->MatPreview);
        }
    }

    void MaterialsPreview::MarkDirty(const WPtr<Material>& material)
    {
        auto it = _previews.find(material);
        if (it != _previews.end())
            it->second->IsDirty = true;
    }

    void MaterialsPreview::DeletePreview(const WPtr<Material>& material)
    { 
        auto it = _previews.find(material);
        if (it != _previews.end())
            _previews.erase(it);
    }

    void MaterialsPreview::SetMeshPreviewType(MeshPreviewType type)
    {
        if (type != _meshPreviewType)
        {
            for (auto& preview : _previews)
                preview.second->IsDirty = true;
        }

        _meshPreviewType = type;
    }

    void MaterialsPreview::DrawMaterial(const WPtr<Material>& material, Preview& preview) const
    { 
        SPtr<Material> mat = material.lock();
        SPtr<Renderable> renderable = nullptr;

        _camera->NotifyNeedsRedraw();
        _camera->GetViewport()->SetTarget(preview.MatPreview->RenderTex);

        _renderer->NotifyRenderablesCleared();
        _renderer->NotifyCameraUpdated(_camera.get(), (UINT32)CameraDirtyFlag::Viewport);

        switch (_meshPreviewType)
        {
        case MeshPreviewType::Box:
            renderable = _boxRenderable;
            break;
        case MeshPreviewType::Plane:
            renderable = _planeRenderable;
            break;
        case MeshPreviewType::Sphere:
            renderable = _sphereRenderable;
            break;
        }

        if (renderable)
        {
            renderable->SetMaterial(mat);
            _renderer->NotifyRenderableAdded(renderable.get());
        }

        _renderer->Update();
        _renderer->RenderAll(*_perFrameData.get());

        preview.IsDirty = false;
    }

    void MaterialsPreview::InitializeCamera()
    {
        _camera = Camera::Create();
        _camera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        _camera->SetAspectRatio(1.0f);
        _camera->SetFlags((UINT32)CameraFlag::OnDemand);
        _camera->GetViewport()->SetClearColorValue(BackgroundColor);

        Transform tfrm = _camera->GetTransform();
        tfrm.Move(Vector3(1.0f, 1.6f, 1.6f));
        tfrm.LookAt(Vector3::ZERO);
        _camera->SetTransform(tfrm);
    }

    void MaterialsPreview::InitializeSkybox()
    {
        _skybox = Skybox::Create();
        _skybox->SetTexture(_radiance);
    }

    void MaterialsPreview::InitializeLight()
    {
        _light = Light::Create(LightType::Radial);
        
        Transform transform = _light->GetTransform();
        transform.Move(Vector3(0.0f, 3.0f, 1.75f));
        _light->SetTransform(transform);
    }

    void MaterialsPreview::InitializeRenderer()
    {
        TE_ASSERT_ERROR(_camera.get(), "Camera must be created before Renderer");
        TE_ASSERT_ERROR(_skybox.get(), "Skybox must be created before Renderer");
        TE_ASSERT_ERROR(_light.get(), "Light must be created before Renderer");
        TE_ASSERT_ERROR(_boxRenderable.get(), "Box must be created before Renderer");
        TE_ASSERT_ERROR(_planeRenderable.get(), "Plane must be created before Renderer");
        TE_ASSERT_ERROR(_sphereRenderable.get(),  "Sphere must be created before Renderer");

        const START_UP_DESC& desc = gCoreApplication().GetStartUpDesc();
        _renderer = RendererManager::Instance().Initialize(desc.Renderer, "Preview", false);

        _renderer->NotifyCameraAdded(_camera.get());
        _renderer->NotifySkyboxAdded(_skybox.get());
        _renderer->NotifyLightAdded(_light.get());

        TE_ASSERT_ERROR(_renderer.get(), "Failed to create renderer");
    }

    void MaterialsPreview::InitializeRenderable()
    {
        auto meshImportOptions = MeshImportOptions::Create();

        _box = ResourceManager::Instance().Load<Mesh>("Data/Meshes/Primitives/cube.obj", meshImportOptions).GetInternalPtr();
        _plane = ResourceManager::Instance().Load<Mesh>("Data/Meshes/Primitives/plane.obj", meshImportOptions).GetInternalPtr();
        _sphere = ResourceManager::Instance().Load<Mesh>("Data/Meshes/Primitives/sphere.obj", meshImportOptions).GetInternalPtr();

        TE_ASSERT_ERROR(_box.get(), "Failed to load box mesh");
        TE_ASSERT_ERROR(_plane.get(), "Failed to load plane mesh");
        TE_ASSERT_ERROR(_sphere.get(), "Failed to load sphere mesh");

        _boxRenderable = Renderable::Create();
        _boxRenderable->SetMesh(_box);
        _planeRenderable = Renderable::Create();
        _planeRenderable->SetMesh(_plane);
        _sphereRenderable = Renderable::Create();
        _sphereRenderable->SetMesh(_sphere);
    }

    void MaterialsPreview::InitializeTextures()
    {
        auto textureCubeMapImportOptions = TextureImportOptions::Create();
        textureCubeMapImportOptions->CpuCached = false;
        textureCubeMapImportOptions->CubemapType = CubemapSourceType::Faces;
        textureCubeMapImportOptions->IsCubemap = true;
        textureCubeMapImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
        textureCubeMapImportOptions->SRGB = true;

        _radiance = ResourceManager::Instance().Load<Texture>("Data/Textures/Skybox/skybox_night_512.png", textureCubeMapImportOptions).GetInternalPtr();
        TE_ASSERT_ERROR(_radiance.get(), "Failed to load envrionment texture");
    }
}
