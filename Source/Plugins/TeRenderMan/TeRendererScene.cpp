#include "TeRendererScene.h"

#include "TeRenderMan.h"
#include "TeRenderManOptions.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeSkybox.h"
#include "Material/TeMaterial.h"
#include "Material/TeShader.h"
#include "Material/TeTechnique.h"
#include "Material/TePass.h"
#include "RenderAPI/TeVertexData.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "Resources/TeBuiltinResources.h"
#include "Mesh/TeMesh.h"
#include "Renderer/TeDecal.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    PerFrameParamDef gPerFrameParamDef;

    /** Returns a specific base pass shader variation. */
    template<bool WRITE_VELOCITY>
    static const ShaderVariation* GetBasePassVariation(bool shaderCanWriteVelocity, RenderableAnimType animType)
    {
        const ShaderVariation* VAR_LOOKUP[2];
        VAR_LOOKUP[0] = &GetVertexInputVariation<false, WRITE_VELOCITY>(shaderCanWriteVelocity);
        VAR_LOOKUP[1] = &GetVertexInputVariation<true, WRITE_VELOCITY>(shaderCanWriteVelocity);

        return VAR_LOOKUP[(int)animType];
    }

    /** Initializes a specific base pass technique on the provided material and returns the technique index. */
    static UINT32 InitAndRetrieveBasePassTechnique(Material& material, bool shaderCanWriteVelocity, bool writeVelocity, RenderableAnimType animType)
    {
        const MaterialProperties& properties = material.GetProperties();
        static const Vector3 Black(0.f, 0.f, 0.f);

        FIND_TECHNIQUE_DESC findDesc;
        findDesc.Override = true;
        findDesc.Variation = writeVelocity ?
            *GetBasePassVariation<true>(shaderCanWriteVelocity, animType) :
            *GetBasePassVariation<false>(shaderCanWriteVelocity, animType);

        findDesc.Variation.AddParam(ShaderVariation::Param("TRANSPARENT", material.GetShader() && material.GetShader()->GetFlags() & (UINT32)ShaderFlag::Transparent));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_BASE_COLOR_MAP", properties.UseBaseColorMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_METALLIC_MAP", properties.UseMetallicMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_ROUGHNESS_MAP", properties.UseRoughnessMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_METALLIC_ROUGHNESS_MAP", properties.UseMetallicRoughnessMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_REFLECTANCE_MAP", properties.UseReflectanceMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_OCCLUSION_MAP", properties.UseOcclusionMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_EMISSIVE_MAP", properties.UseEmissiveMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_SHEEN_COLOR_MAP", properties.UseSheenColorMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_SHEEN_ROUGHNESS_MAP", properties.UseSheenRoughnessMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_CLEAR_COAT_MAP", properties.UseClearCoatMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_CLEAR_COAT_ROUGHNESS_MAP", properties.UseClearCoatRoughnessMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_CLEAR_COAT_NORMAL_MAP", properties.UseClearCoatNormalMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_NORMAL_MAP", properties.UseNormalMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_PARALLAX_MAP", properties.UseParallaxMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_TRANSMISSION_MAP", properties.UseTransmissionMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_OPACITY_MAP", properties.UseOpacityMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_ANISOTROPY_DIRECTION_MAP", properties.UseAnisotropyDirectionMap));
        findDesc.Variation.AddParam(ShaderVariation::Param("DO_INDIRECT_LIGHTING", properties.DoIndirectLighting));
        findDesc.Variation.AddParam(ShaderVariation::Param("DO_DIRECT_LIGHTING", properties.DoDirectLighting));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_SHEEN", properties.UseSheenColorMap || !Math::ApproxEquals(properties.SheenColor.GetAsVector3(), Black)));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_CLEAR_COAT", properties.UseClearCoatMap || !Math::ApproxEquals(properties.ClearCoat, 0.f)));
        findDesc.Variation.AddParam(ShaderVariation::Param("USE_ANISOTROPY", !Math::ApproxEquals(properties.Anisotropy, 0.f)));

        UINT32 techniqueIdx = material.FindTechnique(findDesc, true);

        if (techniqueIdx == (UINT32)-1)
            techniqueIdx = material.GetDefaultTechnique(true);

        TE_ASSERT_ERROR(techniqueIdx != (UINT32)-1, "No technique has been found");

        // Make sure the technique shaders are compiled
        const SPtr<Technique>& technique = material.GetTechnique(techniqueIdx);

        TE_ASSERT_ERROR(technique, "No technique has been found");

        technique->Compile();

        UINT32 numPasses = technique->GetNumPasses();
        TE_ASSERT_ERROR(numPasses > 0, "A technique must a least have one pass");

        return techniqueIdx;
    }

    static void ValidateBasePassMaterial(Material& material, RenderableAnimType animType, UINT32 techniqueIdx, VertexDeclaration& vertexDecl)
    {
        // Validate mesh <-> shader vertex bindings
        UINT32 numPasses = material.GetNumPasses(techniqueIdx);
        for (UINT32 j = 0; j < numPasses; j++)
        {
            const SPtr<Pass> pass = material.GetPass(j, techniqueIdx);
            const SPtr<GraphicsPipelineState>& graphicsPipeline = pass->GetGraphicsPipelineState();

            SPtr<VertexDeclaration> shaderDecl = graphicsPipeline->GetVertexProgram()->GetInputDeclaration();
            if (shaderDecl && !vertexDecl.IsCompatible(shaderDecl))
            {
                Vector<VertexElement> missingElements = vertexDecl.GetMissingElements(shaderDecl);

                if (!missingElements.empty())
                {
                    StringStream wrnStream;
                    wrnStream << "Provided mesh is missing required vertex attributes to render with the provided shader. Missing elements: " << std::endl;

                    for (auto& entry : missingElements)
                        wrnStream << "\t" << ToString(entry.GetSemantic()) << entry.GetSemanticIdx() << std::endl;

                    TE_DEBUG(wrnStream.str());
                    break;
                }
            }
        }
    }

    RendererScene::RendererScene(const SPtr<RenderManOptions>& options)
        : _options(options)
    { 
        _info.PerFrameParamBuffer = gPerFrameParamDef.CreateBuffer();
    }

    RendererScene::~RendererScene()
    { 
        for (auto& entry : _info.Renderables)
            te_delete(entry);

        for (auto& entry : _info.Views)
            te_delete(entry);
    }

    void RendererScene::RegisterCamera(Camera* camera)
    {
        RENDERER_VIEW_DESC viewDesc = CreateViewDesc(camera);

        RendererView* view = te_new<RendererView>(viewDesc);
        view->SetRenderSettings(camera->GetRenderSettings());
        view->UpdatePerViewBuffer();

        UINT32 viewIdx = (UINT32)_info.Views.size();
        _info.Views.push_back(view);

        _info.CameraToView[camera] = viewIdx;
        camera->SetRendererId(viewIdx);

        UpdateCameraRenderTargets(camera);
    }

    void RendererScene::UpdateCamera(Camera* camera, UINT32 updateFlag)
    {
        UINT32 cameraId = camera->GetRendererId();
        RendererView* view = _info.Views[cameraId];

        if ((updateFlag & (UINT32)CameraDirtyFlag::Redraw) != 0)
            view->NotifyNeedsRedraw();

        if ((updateFlag & (UINT32)CameraDirtyFlag::RenderSettings) != 0)
            view->SetRenderSettings(camera->GetRenderSettings());

        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything
            | (UINT32)CameraDirtyFlag::Viewport;

        if ((updateFlag & updateEverythingFlag) != 0)
        {
            RENDERER_VIEW_DESC viewDesc = CreateViewDesc(camera);

            view->SetView(viewDesc);
            view->NotifyNeedsRedraw();

            UpdateCameraRenderTargets(camera);
        }

        const Transform& tfrm = camera->GetTransform();
        view->SetTransform(
            tfrm.GetPosition(),
            tfrm.GetForward(),
            camera->GetViewMatrix(),
            camera->GetProjectionMatrixRS(),
            camera->GetWorldFrustum());

        view->UpdatePerViewBuffer();
    }

    void RendererScene::UnregisterCamera(Camera* camera)
    {
        UINT32 cameraId = camera->GetRendererId();

        if (_info.CameraToView.find(camera) == _info.CameraToView.end())
            return;

        Camera* lastCamera = _info.Views.back()->GetSceneCamera();
        UINT32 lastCameraId = lastCamera->GetRendererId();

        if (cameraId != lastCameraId)
        {
            // Swap current last element with the one we want to erase
            std::swap(_info.Views[cameraId], _info.Views[lastCameraId]);
            lastCamera->SetRendererId(cameraId);

            _info.CameraToView[lastCamera] = cameraId;
        }

        // Last element is the one we want to erase
        RendererView* view = _info.Views[_info.Views.size() - 1];
        te_delete(view);

        _info.Views.erase(_info.Views.end() - 1);

        auto iterFind = _info.CameraToView.find(camera);
        if (iterFind != _info.CameraToView.end())
            _info.CameraToView.erase(iterFind);

        UpdateCameraRenderTargets(camera, true);
    }

    void RendererScene::ClearCameras()
    {
        for (auto& camera : _info.CameraToView)
        {
            UpdateCameraRenderTargets(const_cast<Camera*>(camera.first), true);
        }

        for (auto& view : _info.Views)
        {
            te_delete(view);
        }

        _info.Views.clear();
        _info.CameraToView.clear();
    }

    void RendererScene::RegisterLight(Light* light)
    {
        if (light->GetType() == Light::Type::Directional)
        {
            UINT32 lightId = (UINT32)_info.DirectionalLights.size();
            light->SetRendererId(lightId);

            _info.DirectionalLights.push_back(RendererLight(light));
        }
        else
        {
            if (light->GetType() == Light::Type::Radial)
            {
                UINT32 lightId = (UINT32)_info.RadialLights.size();
                light->SetRendererId(lightId);

                _info.RadialLights.push_back(RendererLight(light));
                _info.RadialLightWorldBounds.push_back(light->GetBounds());
            }
            else // Spot
            {
                UINT32 lightId = (UINT32)_info.SpotLights.size();
                light->SetRendererId(lightId);

                _info.SpotLights.push_back(RendererLight(light));
                _info.SpotLightWorldBounds.push_back(light->GetBounds());
            }
        }
    }

    void RendererScene::UpdateLight(Light* light)
    {
        UINT32 lightId = light->GetRendererId();

        if (light->GetType() == Light::Type::Radial)
            _info.RadialLightWorldBounds[lightId] = light->GetBounds();
        else if (light->GetType() == Light::Type::Spot)
            _info.SpotLightWorldBounds[lightId] = light->GetBounds();
    }

    void RendererScene::UnregisterLight(Light* light)
    {
        UINT32 lightId = light->GetRendererId();
        if (light->GetType() == Light::Type::Directional)
        {
            if (_info.DirectionalLights.size() <= lightId)
                return;
            if (_info.DirectionalLights[lightId]._internal != light)
                return;

            Light* lastLight = _info.DirectionalLights.back()._internal;
            UINT32 lastLightId = lastLight->GetRendererId();

            if (lightId != lastLightId)
            {
                // Swap current last element with the one we want to erase
                std::swap(_info.DirectionalLights[lightId], _info.DirectionalLights[lastLightId]);
                lastLight->SetRendererId(lightId);
            }

            // Last element is the one we want to erase
            _info.DirectionalLights.erase(_info.DirectionalLights.end() - 1);
        }
        else
        {
            if (light->GetType() == Light::Type::Radial)
            {
                if (_info.RadialLights.size() <= lightId)
                    return;
                if (_info.RadialLights[lightId]._internal != light)
                    return;

                Light* lastLight = _info.RadialLights.back()._internal;
                UINT32 lastLightId = lastLight->GetRendererId();

                if (lightId != lastLightId)
                {
                    // Swap current last element with the one we want to erase
                    std::swap(_info.RadialLights[lightId], _info.RadialLights[lastLightId]);
                    std::swap(_info.RadialLightWorldBounds[lightId], _info.RadialLightWorldBounds[lastLightId]);

                    lastLight->SetRendererId(lightId);
                }

                // Last element is the one we want to erase
                _info.RadialLights.erase(_info.RadialLights.end() - 1);
                _info.RadialLightWorldBounds.erase(_info.RadialLightWorldBounds.end() - 1);
            }
            else
            {
                if (_info.SpotLights.size() <= lightId)
                    return;
                if (_info.SpotLights[lightId]._internal != light)
                    return;

                Light* lastLight = _info.SpotLights.back()._internal;
                UINT32 lastLightId = lastLight->GetRendererId();

                if (lightId != lastLightId)
                {
                    // Swap current last element with the one we want to erase
                    std::swap(_info.SpotLights[lightId], _info.SpotLights[lastLightId]);
                    std::swap(_info.SpotLightWorldBounds[lightId], _info.SpotLightWorldBounds[lastLightId]);

                    lastLight->SetRendererId(lightId);
                }

                // Last element is the one we want to erase
                _info.SpotLights.erase(_info.SpotLights.end() - 1);
                _info.SpotLightWorldBounds.erase(_info.SpotLightWorldBounds.end() - 1);
            }
        }
    }

    void RendererScene::ClearLights()
    {
        _info.DirectionalLights.clear();
        _info.RadialLights.clear();
        _info.RadialLightWorldBounds.clear();
        _info.SpotLights.clear();
        _info.SpotLightWorldBounds.clear();
    }

    void RendererScene::RegisterRenderable(Renderable* renderable)
    { 
        UINT32 renderableId = (UINT32)_info.Renderables.size();

        renderable->SetRendererId(renderableId);
        _info.Renderables.push_back(te_new<RendererRenderable>());
        _info.RenderableCullInfos.push_back(CullInfo(renderable->GetBounds(), renderable->GetLayer(), renderable->GetCullDistanceFactor()));

        RendererRenderable* rendererRenderable = _info.Renderables.back();
        rendererRenderable->RenderablePtr = renderable;
        rendererRenderable->WorldTfrm = renderable->GetMatrix();
        rendererRenderable->PrevWorldTfrm = rendererRenderable->WorldTfrm;
        rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::Clean;
        rendererRenderable->UpdatePerObjectBuffer();

        SetMeshData(rendererRenderable, renderable);

        if (_options->InstancingMode == RenderManInstancing::Manual)
        {
            auto iter = std::find(_info.RenderablesInstanced.begin(), _info.RenderablesInstanced.end(), rendererRenderable);
            if (renderable->GetInstancing() && iter == _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.push_back(rendererRenderable);
            else if (!renderable->GetInstancing() && iter != _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.erase(iter);
        }
    }

    void RendererScene::UpdateRenderable(Renderable* renderable)
    {
        UINT32 renderableId = renderable->GetRendererId();

        // If element can be merged, we check if it still exist on renderer side
        if (renderable->GetCanBeMerged())
        {
            if (renderableId > _info.Renderables.size())
                return;
            if (_info.Renderables[renderableId]->RenderablePtr != renderable)
                return;
        }

        RendererRenderable* rendererRenderable = _info.Renderables[renderableId];

        if(rendererRenderable->PreviousFrameDirtyState != PrevFrameDirtyState::Updated)
            rendererRenderable->PrevWorldTfrm = rendererRenderable->WorldTfrm;

        rendererRenderable->WorldTfrm = renderable->GetMatrix();
        rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::Updated;

        _info.Renderables[renderableId]->UpdatePerObjectBuffer();
        _info.RenderableCullInfos[renderableId].Layer = renderable->GetLayer();
        _info.RenderableCullInfos[renderableId].Boundaries = renderable->GetBounds();
        _info.RenderableCullInfos[renderableId].CullDistanceFactor = renderable->GetCullDistanceFactor();

        if (_options->InstancingMode == RenderManInstancing::Manual)
        {
            auto iter = std::find(_info.RenderablesInstanced.begin(), _info.RenderablesInstanced.end(), rendererRenderable);
            if (renderable->GetInstancing() && iter == _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.push_back(rendererRenderable);
            else if (!renderable->GetInstancing() && iter != _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.erase(iter);
        }

        UINT32 dirtyFlag = renderable->GetCoreDirtyFlags();
        if (dirtyFlag & (UINT32)ActorDirtyFlag::GpuParams)
            SetMeshData(rendererRenderable, renderable);
    }

    void RendererScene::UnregisterRenderable(Renderable* renderable)
    {
        UINT32 renderableId = renderable->GetRendererId();

        if (_info.Renderables.size() <= renderableId)
            return;
        if (_info.Renderables[renderableId]->RenderablePtr != renderable)
            return;

        // If element can be merged, we check if it still exist on renderer side
        if (renderable->GetCanBeMerged())
        {
            if (renderableId > _info.Renderables.size())
                return;
        }

        Renderable* lastRenderable = _info.Renderables.back()->RenderablePtr;
        UINT32 lastRenderableId = lastRenderable->GetRendererId();

        RendererRenderable* rendererRenderable = _info.Renderables[renderableId];
        
        if (renderableId != lastRenderableId)
        {
            // Swap current last element with the one we want to erase
            std::swap(_info.Renderables[renderableId], _info.Renderables[lastRenderableId]);
            std::swap(_info.RenderableCullInfos[renderableId], _info.RenderableCullInfos[lastRenderableId]);

            lastRenderable->SetRendererId(renderableId);
        }

        if (_options->InstancingMode == RenderManInstancing::Manual)
        {
            auto iter = std::find(_info.RenderablesInstanced.begin(), _info.RenderablesInstanced.end(), rendererRenderable);
            if (iter != _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.erase(iter);
        }

        // Last element is the one we want to erase
        _info.Renderables.erase(_info.Renderables.end() - 1);
        _info.RenderableCullInfos.erase(_info.RenderableCullInfos.end() - 1);

        te_delete(rendererRenderable);
    }

    void RendererScene::ClearRenderables()
    {
        for (auto& rendererRenderable : _info.Renderables)
        {
            te_delete(rendererRenderable);
        }

        _info.Renderables.clear();
        _info.RenderablesInstanced.clear();
        _info.RenderableCullInfos.clear();
    }

    void RendererScene::RegisterDecal(Decal* decal)
    {
        const auto renderableId = (UINT32)_info.Decals.size();
        decal->SetRendererId(renderableId);

        _info.Decals.emplace_back();
        _info.DecalCullInfos.push_back(CullInfo(decal->GetBounds(), decal->GetLayer()));

        RendererDecal& rendererDecal = _info.Decals.back();
        rendererDecal.DecalPtr = decal;
        rendererDecal.UpdatePerObjectBuffer();

        DecalRenderElement& renElement = rendererDecal.Element;
        renElement.Type = (UINT32)RenderElementType::Decal;
        renElement.MeshElem = gRendererUtility().GetBoxStencil();
        renElement.SubMeshElem = &renElement.MeshElem->GetProperties().GetSubMesh();

        renElement.MaterialElem = decal->GetMaterial();

        if (renElement.MaterialElem != nullptr && renElement.MaterialElem->GetShader() == nullptr)
            renElement.MaterialElem = nullptr;

        // If no material use the default material
        if (renElement.MaterialElem == nullptr)
            renElement.MaterialElem = Material::Create(DefaultDecalMat::Get()->GetShader()).GetInternalPtr();

        // TODO decal
    }

    void RendererScene::UpdateDecal(Decal* decal)
    {
        const UINT32 rendererId = decal->GetRendererId();

        _info.Decals[rendererId].UpdatePerObjectBuffer();
        _info.DecalCullInfos[rendererId].Boundaries = decal->GetBounds();
    }

    void RendererScene::UnregisterDecal(Decal* decal)
    {
        const UINT32 decalId = decal->GetRendererId();

        if (_info.Decals.size() <= decalId)
            return;
        if (_info.Decals[decalId].DecalPtr != decal)
            return;

        Decal* lastDecal = _info.Decals.back().DecalPtr;
        const UINT32 lastDecalId = lastDecal->GetRendererId();

        if (decalId != lastDecalId)
        {
            // Swap current last element with the one we want to erase
            std::swap(_info.Decals[decalId], _info.Decals[lastDecalId]);
            std::swap(_info.DecalCullInfos[decalId], _info.DecalCullInfos[lastDecalId]);

            lastDecal->SetRendererId(decalId);
        }

        // Last element is the one we want to erase
        _info.Decals.erase(_info.Decals.end() - 1);
        _info.DecalCullInfos.erase(_info.DecalCullInfos.end() - 1);
    }

    void RendererScene::ClearDecals()
    {
        _info.Decals.clear();
        _info.DecalCullInfos.clear();
    }

    void RendererScene::BatchRenderables()
    { 
        // TODO batch renderables
    }

    void RendererScene::DestroyBatchedRenderables()
    {
        // TODO batch renderables
    }

    void RendererScene::SetMeshData(RendererRenderable* rendererRenderable, Renderable* renderable)
    {
        SPtr<Mesh> mesh = renderable->GetMesh();
        if (mesh != nullptr)
        {
            MeshProperties& meshProps = mesh->GetProperties();
            SPtr<VertexDeclaration> vertexDecl = mesh->GetVertexData()->vertexDeclaration;

            for (UINT32 i = 0; i < meshProps.GetNumSubMeshes(); i++)
            {
                RenderableElement* renElement = nullptr;

                if (rendererRenderable->Elements.size() == i)
                    rendererRenderable->Elements.push_back(RenderableElement());

                renElement = &rendererRenderable->Elements[i];

                renElement->Type = (UINT32)RenderElementType::Renderable;
                renElement->MeshElem = mesh;
                renElement->ZPrepassMeshElem = renderable->GetZPrepassMesh();
                renElement->SubMeshElem = meshProps.GetSubMeshPtr(i);
                renElement->BoneMatrixBuffer = renderable->GetBoneMatrixBuffer();
                renElement->BonePrevMatrixBuffer = renderable->GetBonePrevMatrixBuffer();
                renElement->AnimType = renderable->GetAnimType();
                renElement->AnimationId = renderable->GetAnimationId();
                renElement->UseForZPrepass = renderable->GetUseForZPrepass();

                renElement->MaterialElem = renderable->GetMaterial(i);
                if (renElement->MaterialElem == nullptr)
                    renElement->MaterialElem = nullptr;

                if (renElement->MaterialElem != nullptr && renElement->MaterialElem->GetShader() == nullptr)
                    renElement->MaterialElem = nullptr;

                // If no material use the default material
                if (renElement->MaterialElem == nullptr)
                    renElement->MaterialElem = gBuiltinResources().GetDefaultMaterial().GetInternalPtr();

                const SPtr<Shader>& shader = renElement->MaterialElem->GetShader();

                const Vector<ShaderVariationParamInfo>& variationParams = shader->GetVariationParams();
                const bool shaderCanWriteVelocity = std::find_if(variationParams.begin(), variationParams.end(),
                    [](const ShaderVariationParamInfo& x) { return x.Identifier == "WRITE_VELOCITY"; }) != variationParams.end();

                const bool writeVelocity = shaderCanWriteVelocity && renderable->GetWriteVelocity();

                RenderableAnimType animType = renderable->GetAnimType();

                // Determine which technique to use
                renElement->DefaultTechniqueIdx = InitAndRetrieveBasePassTechnique(*renElement->MaterialElem, shaderCanWriteVelocity, false, animType);

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
                ValidateBasePassMaterial(*renElement->MaterialElem, animType, renElement->DefaultTechniqueIdx, *vertexDecl);
#endif

                // Generate or assigned renderer specific data for the material
                renElement->MaterialElem->CreateGpuParams(renElement->DefaultTechniqueIdx, renElement->GpuParamsElem);

                if (writeVelocity)
                {
                    renElement->WriteVelocityTechniqueIdx = InitAndRetrieveBasePassTechnique(*renElement->MaterialElem, shaderCanWriteVelocity, true, animType);

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
                    ValidateBasePassMaterial(*renElement->MaterialElem, animType, renElement->WriteVelocityTechniqueIdx, *vertexDecl);
#endif

                    // Note: Using the same params as the non-velocity technique. There are assumed to be no differences
                }
                else
                {
                    renElement->WriteVelocityTechniqueIdx = (UINT32)-1;
                }

                // We update gpu paremeters such as diffuse or specular defined for this material
                PerObjectBuffer::UpdatePerMaterial(renElement->PerMaterialParamBuffer, renElement->MaterialElem->GetProperties());

                // Set renderable properties to renderElement
                renElement->Properties = &renderable->GetProperties();
            }

            // Prepare all parameter bindings
            for (auto& element : rendererRenderable->Elements)
            {
                SPtr<Shader> shader = element.MaterialElem->GetShader();
                if (shader == nullptr)
                {
                    TE_DEBUG("Missing shader on material.");
                    continue;
                }

                for (auto& gpuParams : element.GpuParamsElem)
                {
                    gpuParams->SetParamBlockBuffer("PerObjectBuffer", rendererRenderable->PerObjectParamBuffer);
                    gpuParams->SetParamBlockBuffer("PerMaterialBuffer", element.PerMaterialParamBuffer);

                    if (gpuParams->HasBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices"))
                        gpuParams->SetBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices", element.BoneMatrixBuffer);

                    if (gpuParams->HasBuffer(GPT_VERTEX_PROGRAM, "PrevBoneMatrices"))
                        gpuParams->SetBuffer(GPT_VERTEX_PROGRAM, "PrevBoneMatrices", element.BonePrevMatrixBuffer);
                }
            }
        }
    }

    void RendererScene::RegisterSkybox(Skybox* skybox)
    {
        _info.SkyboxElem = skybox;
    }

    void RendererScene::UnregisterSkybox(Skybox* skybox)
    {
        if (_info.SkyboxElem == skybox)
            _info.SkyboxElem = nullptr;
    }

    void RendererScene::ClearSkybox()
    {
        _info.SkyboxElem = nullptr;
    }

    void RendererScene::SetOptions(const SPtr<RenderManOptions>& options)
    {
        _options = options;

        for (auto& entry : _info.Views)
            entry->SetStateReductionMode(_options->ReductionMode);
    }

    void RendererScene::SetParamFrameParams(const float& time, const float& delta)
    {
        gPerFrameParamDef.gTime.Set(_info.PerFrameParamBuffer, time);
        gPerFrameParamDef.gFrameDelta.Set(_info.PerFrameParamBuffer, delta);
    }

    void RendererScene::SetParamCameraParams(const Color& sceneLightColor)
    {
        gPerFrameParamDef.gSceneLightColor.Set(_info.PerFrameParamBuffer, sceneLightColor.GetAsVector4());
    }

    void RendererScene::SetParamSkyboxParams(bool enabled)
    {
        UINT32 SkyboxNumMips = 0;
        SPtr<Texture> prefilteredRadiance = nullptr;

        if(_info.SkyboxElem != nullptr && enabled)
        {
            prefilteredRadiance = _info.SkyboxElem->GetSpecularIrradiance();

            gPerFrameParamDef.gSkyboxBrightness.Set(_info.PerFrameParamBuffer, _info.SkyboxElem->GetBrightness());
            gPerFrameParamDef.gSkyboxIBLIntensity.Set(_info.PerFrameParamBuffer, _info.SkyboxElem->GetIBLIntensity());
            gPerFrameParamDef.gUseSkyboxMap.Set(_info.PerFrameParamBuffer, _info.SkyboxElem->GetTexture() ? 1 : 0);
            gPerFrameParamDef.gUseSkyboxDiffuseIrrMap.Set(_info.PerFrameParamBuffer, _info.SkyboxElem->GetDiffuseIrradiance() ? 1 : 0);
            gPerFrameParamDef.gUseSkyboxPrefilteredRadianceMap.Set(_info.PerFrameParamBuffer, prefilteredRadiance ? 1 : 0);

            if(prefilteredRadiance)
                SkyboxNumMips = prefilteredRadiance->GetProperties().GetNumMipmaps();

            gPerFrameParamDef.gSkyboxNumMips.Set(_info.PerFrameParamBuffer, SkyboxNumMips);
        }
        else
        {
            gPerFrameParamDef.gSkyboxBrightness.Set(_info.PerFrameParamBuffer, 1.0f);
            gPerFrameParamDef.gSkyboxIBLIntensity.Set(_info.PerFrameParamBuffer, Skybox::DEFAULT_IBL_INTENSITY);
            gPerFrameParamDef.gUseSkyboxMap.Set(_info.PerFrameParamBuffer, 0);
            gPerFrameParamDef.gUseSkyboxDiffuseIrrMap.Set(_info.PerFrameParamBuffer, 0);
            gPerFrameParamDef.gUseSkyboxPrefilteredRadianceMap.Set(_info.PerFrameParamBuffer, 0);
        }
    }

    void RendererScene::SetParamHDRParams(bool useGamma, bool useToneMapping, float gamma, float exposure, float contrast, float brightness)
    {
        gPerFrameParamDef.gUseGamma.Set(_info.PerFrameParamBuffer, useGamma, 0);
        gPerFrameParamDef.gUseToneMapping.Set(_info.PerFrameParamBuffer, useToneMapping, 0);
        gPerFrameParamDef.gGamma.Set(_info.PerFrameParamBuffer, gamma, 0);
        gPerFrameParamDef.gExposure.Set(_info.PerFrameParamBuffer, exposure, 0);
        gPerFrameParamDef.gContrast.Set(_info.PerFrameParamBuffer, contrast, 0);
        gPerFrameParamDef.gBrightness.Set(_info.PerFrameParamBuffer, brightness, 0);
    }

    void RendererScene::PrepareRenderable(UINT32 idx, const FrameInfo& frameInfo)
    {
        RendererRenderable* rendererRenderable = _info.Renderables[idx];

        if (frameInfo.FrameDatas.Animation != nullptr)
            rendererRenderable->RenderablePtr->UpdatePrevFrameAnimationBuffers();

        if (rendererRenderable->PreviousFrameDirtyState != PrevFrameDirtyState::Clean)
        {
            if (rendererRenderable->PreviousFrameDirtyState == PrevFrameDirtyState::Updated)
                rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::CopyMostRecent;
            else if (rendererRenderable->PreviousFrameDirtyState == PrevFrameDirtyState::CopyMostRecent)
            {
                rendererRenderable->PrevWorldTfrm = _info.Renderables[idx]->WorldTfrm;
                rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::Clean;
                rendererRenderable->UpdatePerObjectBuffer();
            }
        }
    }

    void RendererScene::PrepareVisibleRenderable(UINT32 idx, const FrameInfo& frameInfo)
    {
        if (_info.RenderableReady[idx])
            return;

        RendererRenderable* rendererRenderable = _info.Renderables[idx];

        if(frameInfo.FrameDatas.Animation != nullptr)
            rendererRenderable->RenderablePtr->UpdateAnimationBuffers(*frameInfo.FrameDatas.Animation);

        _info.RenderableReady[idx] = true;
    }

    RENDERER_VIEW_DESC RendererScene::CreateViewDesc(Camera* camera) const
    {
        SPtr<Viewport> viewport = camera->GetViewport();
        RENDERER_VIEW_DESC viewDesc;

        viewDesc.Target.ClearFlags = viewport->GetClearFlags();

        viewDesc.Target.ClearColor = viewport->GetClearColorValue();
        viewDesc.Target.ClearDepthValue = viewport->GetClearDepthValue();
        viewDesc.Target.ClearStencilValue = viewport->GetClearStencilValue();

        viewDesc.Target.Target = viewport->GetTarget();
        viewDesc.Target.NrmViewRect = viewport->GetArea();
        viewDesc.Target.ViewRect = viewport->GetPixelArea();

        if (viewDesc.Target.Target != nullptr)
        {
            viewDesc.Target.TargetWidth = viewDesc.Target.Target->GetProperties().Width;
            viewDesc.Target.TargetHeight = viewDesc.Target.Target->GetProperties().Height;
        }
        else
        {
            viewDesc.Target.TargetWidth = 0;
            viewDesc.Target.TargetHeight = 0;
        }

        viewDesc.Target.NumSamples = camera->GetMSAACount();

        viewDesc.MainView = camera->IsMain();

        UINT32 flag = (UINT32)camera->GetFlags();
        viewDesc.RunPostProcessing = true;
        viewDesc.OnDemand = (flag & (UINT32)CameraFlag::OnDemand) ? true : false;

        viewDesc.CullFrustum = camera->GetWorldFrustum();
        viewDesc.VisibleLayers = camera->GetLayers();
        viewDesc.NearPlane = camera->GetNearClipDistance();
        viewDesc.FarPlane = camera->GetFarClipDistance();
        viewDesc.FlipView = false;

        const Transform& tfrm = camera->GetTransform();
        viewDesc.ViewOrigin = tfrm.GetPosition();
        viewDesc.ViewDirection = tfrm.GetForward();
        viewDesc.ProjTransform = camera->GetProjectionMatrixRS();
        viewDesc.ViewTransform = camera->GetViewMatrix();
        viewDesc.ProjType = camera->GetProjectionType();

        viewDesc.ReductionMode = _options->ReductionMode;
        viewDesc.SceneCamera = camera;

        return viewDesc;
    }

    void RendererScene::UpdateCameraRenderTargets(Camera* camera, bool remove)
    {
        SPtr<RenderTarget> renderTarget = camera->GetViewport()->GetTarget();

        // Remove from render target list
        int rtChanged = 0; // 0 - No RT, 1 - RT found, 2 - RT changed
        for (auto iterTarget = _info.RenderTargets.begin(); iterTarget != _info.RenderTargets.end(); ++iterTarget)
        {
            RendererRenderTarget& target = *iterTarget;
            for (auto iterCam = target.Cameras.begin(); iterCam != target.Cameras.end(); ++iterCam)
            {
                if (camera == *iterCam)
                {
                    if (remove)
                    {
                        target.Cameras.erase(iterCam);
                        rtChanged = 1;
                    }
                    else
                    {
                        if (renderTarget != target.Target)
                        {
                            target.Cameras.erase(iterCam);
                            rtChanged = 2;
                        }
                        else
                            rtChanged = 1;
                    }

                    break;
                }
            }

            if (target.Cameras.empty())
            {
                _info.RenderTargets.erase(iterTarget);
                break;
            }
        }

        // Register in render target list
        if (renderTarget != nullptr && !remove && (rtChanged == 0 || rtChanged == 2))
        {
            auto findIter = std::find_if(_info.RenderTargets.begin(), _info.RenderTargets.end(),
                [&](const RendererRenderTarget& x) { return x.Target == renderTarget; });

            if (findIter != _info.RenderTargets.end())
            {
                findIter->Cameras.push_back(camera);
            }
            else
            {
                _info.RenderTargets.push_back(RendererRenderTarget());
                RendererRenderTarget& renderTargetData = _info.RenderTargets.back();

                renderTargetData.Target = renderTarget;
                renderTargetData.Cameras.push_back(camera);
            }

            // Sort render targets based on priority
            auto cameraComparer = [&](const Camera* a, const Camera* b) { return a->GetPriority() > b->GetPriority(); };
            auto renderTargetInfoComparer = [&](const RendererRenderTarget& a, const RendererRenderTarget& b)
            { return a.Target->GetProperties().Priority > b.Target->GetProperties().Priority; };
            std::sort(begin(_info.RenderTargets), end(_info.RenderTargets), renderTargetInfoComparer);

            for (auto& camerasPerTarget : _info.RenderTargets)
            {
                Vector<Camera*>& cameras = camerasPerTarget.Cameras;
                std::sort(begin(cameras), end(cameras), cameraComparer);
            }
        }
    }
}
