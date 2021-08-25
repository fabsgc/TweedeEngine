#include "TeMaterialsPreview.h"

#include "Mesh/TeMesh.h"
#include "Renderer/TeCamera.h"
#include "Material/TeMaterial.h"
#include "TeMaterialsPreviewMat.h"
#include "../TeEditorResManager.h"
#include "Importer/TeMeshImportOptions.h"

namespace te
{
    MaterialsPreview::Preview::Preview()
    { 
        MatPreview = te_shared_ptr_new<RendererUtility::RenderTextureData>();

        MatPreview->Width = 300;
        MatPreview->Height = 200;

        gRendererUtility().GenerateViewportRenderTexture(*MatPreview);
    }

    MaterialsPreview::Preview::~Preview()
    {
        if (MatPreview->RenderTex)
            MatPreview->RenderTex = nullptr;
        if (MatPreview->ColorTex.IsLoaded())
            MatPreview->ColorTex.Release();
        if (MatPreview->DepthStencilTex.IsLoaded())
            MatPreview->DepthStencilTex.Release();
    }

    MaterialsPreview::MaterialsPreview()
        : _opaqueMat(nullptr)
        , _transparentMat(nullptr)
        , _mesh(nullptr)
        , _camera(nullptr)
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

    void MaterialsPreview::DrawMaterial(const WPtr<Material>& material, Preview& preview)
    { 
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_DEPTH;
        PreviewMat* previewMat = nullptr;
        SPtr<Material> mat = material.lock();

        rapi.SetRenderTarget(preview.MatPreview->RenderTex);
        rapi.ClearViewport(clearBuffers, Color::Black);

        if (mat->GetShader()->GetName() == "ForwardOpaque") previewMat = _opaqueMat;
        else previewMat = _transparentMat;

        previewMat->BindFrame();
        previewMat->BindLight();
        previewMat->BindObject();
        previewMat->BindCamera(_camera);
        previewMat->BindMaterial(material);

        if (_mesh)
        {
            if (mat->GetShader()->GetName() == "ForwardOpaque") _opaqueMat->Bind();
            else _transparentMat->Bind();

            MeshProperties properties = _mesh->GetProperties();
            UINT32 numMeshes = properties.GetNumSubMeshes();

            for (UINT32 i = 0; i < numMeshes; i++)
                gRendererUtility().Draw(_mesh, properties.GetSubMesh(i), 1);
        }

        rapi.SetRenderTarget(nullptr);
        preview.IsDirty = false;
    }

    void MaterialsPreview::InitializeCamera()
    {
        _camera = Camera::Create();
        _camera->GetViewport()->SetClearColorValue(Color(0.42f, 0.67f, 0.94f, 1.0f));
        _camera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        _camera->Initialize();
    }

    void MaterialsPreview::InitializeRenderable()
    {
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = true;
        meshImportOptions->ImportTangents = true;
        meshImportOptions->CpuCached = false;

        _mesh = EditorResManager::Instance().Load<Mesh>("Data/Meshes/Sphere/sphere.obj", meshImportOptions).GetInternalPtr();
    }
}
