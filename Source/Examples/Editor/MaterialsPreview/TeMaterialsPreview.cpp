#include "TeMaterialsPreview.h"

#include "Mesh/TeMesh.h"
#include "Image/TeTexture.h"
#include "Renderer/TeCamera.h"
#include "Material/TeMaterial.h"
#include "TeMaterialsPreviewMat.h"
#include "Resources/TeResourceManager.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"

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
        : _opaqueMat(nullptr)
        , _transparentMat(nullptr)
        , _box(nullptr)
        , _plane(nullptr)
        , _sphere(nullptr)
        , _camera(nullptr)
        , _meshPreviewType(MeshPreviewType::Sphere)
    { 
        _opaqueMat = PreviewOpaqueMat::Get();
        _transparentMat = PreviewTransparentMat::Get();

        InitializeCamera();
        InitializeRenderable();
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
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;
        PreviewMat* previewMat = nullptr;
        SPtr<Material> mat = material.lock();
        SPtr<Mesh> mesh = nullptr;

        rapi.SetRenderTarget(preview.MatPreview->RenderTex);
        rapi.ClearViewport(clearBuffers, MaterialsPreview::BackgroundColor);

        if (mat->GetShader()->GetName() == "ForwardOpaque") 
            previewMat = _opaqueMat;
        else 
            previewMat = _transparentMat;

        previewMat->BindFrame();
        previewMat->BindLight();
        previewMat->BindObject();
        previewMat->BindCamera(_camera);
        previewMat->BindMaterial(material);

        switch (_meshPreviewType)
        {
        case MeshPreviewType::Box:
            mesh = _box;
            break;
        case MeshPreviewType::Plane:
            mesh = _plane;
            break;
        case MeshPreviewType::Sphere:
            mesh = _sphere;
            break;
        }
        
        if (mesh)
        {
            if (mat->GetShader()->GetName() == "ForwardOpaque")
            {
                _opaqueMat->BindTextures(material, _irradiance, _environment);
                _opaqueMat->Bind();
            }
            else
            {
                _transparentMat->BindTextures(material, _irradiance, _environment);
                _transparentMat->Bind();
            }

            MeshProperties properties = mesh->GetProperties();
            UINT32 numMeshes = properties.GetNumSubMeshes();

            for (UINT32 i = 0; i < numMeshes; i++)
                gRendererUtility().Draw(mesh, properties.GetSubMesh(i), 1);
        }

        rapi.SetRenderTarget(nullptr);
        preview.IsDirty = false;
    }

    void MaterialsPreview::InitializeCamera()
    {
        _camera = Camera::Create();
        _camera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        _camera->SetAspectRatio(1.0f);
        _camera->Initialize();

        Transform tfrm = _camera->GetTransform();
        tfrm.Move(Vector3(1.0f, 1.6f, 1.6f));
        tfrm.LookAt(Vector3::ZERO);
        _camera->SetTransform(tfrm);
    }

    void MaterialsPreview::InitializeRenderable()
    {
        auto meshImportOptions = MeshImportOptions::Create();

        auto textureCubeMapImportOptions = TextureImportOptions::Create();
        textureCubeMapImportOptions->CpuCached = false;
        textureCubeMapImportOptions->CubemapType = CubemapSourceType::Faces;
        textureCubeMapImportOptions->IsCubemap = true;
        textureCubeMapImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

        _box = ResourceManager::Instance().Load<Mesh>("Data/Meshes/Primitives/cube.obj", meshImportOptions).GetInternalPtr();
        _plane = ResourceManager::Instance().Load<Mesh>("Data/Meshes/Primitives/plane.obj", meshImportOptions).GetInternalPtr();
        _sphere = ResourceManager::Instance().Load<Mesh>("Data/Meshes/Primitives/sphere.obj", meshImportOptions).GetInternalPtr();
        _irradiance = ResourceManager::Instance().Load<Texture>("Data/Textures/Skybox/skybox_day_irradiance_small.png", textureCubeMapImportOptions).GetInternalPtr();
        _environment = ResourceManager::Instance().Load<Texture>("Data/Textures/Skybox/skybox_day_medium.png", textureCubeMapImportOptions).GetInternalPtr();
    }
}
