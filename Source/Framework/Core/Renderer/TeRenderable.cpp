#include "TeRenderable.h"

#include "TeRenderer.h"
#include "Mesh/TeMesh.h"
#include "Scene/TeSceneObject.h"
#include "Animation/TeAnimation.h"
#include "Animation/TeAnimationManager.h"
#include "RenderAPI/TeGpuBuffer.h"

namespace te
{
    SPtr<GpuBuffer> CreateBoneMatrixBuffer(UINT32 numBones)
    {
        GPU_BUFFER_DESC desc;
        desc.ElementCount = numBones * 4;
        desc.ElementSize = 0;
        desc.Type = GBT_STANDARD;
        desc.Format = BF_32X4F;
        desc.Usage = GBU_DYNAMIC;

        SPtr<GpuBuffer> buffer = GpuBuffer::Create(desc);
        UINT8* dest = (UINT8*)buffer->Lock(0, numBones * 4 * sizeof(Vector4), GBL_WRITE_ONLY_DISCARD);

        // Initialize bone transforms to identity, so the object renders properly even if no animation is animating it
        for (UINT32 i = 0; i < numBones; i++)
        {
            memcpy(dest, &Matrix4::IDENTITY, 16 * sizeof(float)); // Assuming row-major format
            dest += 16 * sizeof(float);
        }

        buffer->Unlock();

        return buffer;
    }

    Renderable::Renderable()
        : Serializable(TID_Renderable)
        , _rendererId(0)
        , _animationId((UINT64)-1)
        , _boundsDirty(true)
        , _subMeshesBoundsDirty(true)
    { }

    Renderable::~Renderable()
    {
        if(_active)
        {
            if (_renderer) _renderer->NotifyRenderableRemoved(this);
        }
    }

    void Renderable::Initialize()
    {
        if (_renderer) _renderer->NotifyRenderableAdded(this);
        CoreObject::Initialize();
    }

    void Renderable::OnMeshChanged()
    {
        RefreshAnimation();
        _markCoreDirty();
    }

    void Renderable::RefreshAnimation()
    {
        if (_animation == nullptr)
        {
            _animType = RenderableAnimType::None;
            return;
        }

        if (_mesh)
        {
            SPtr<Skeleton> skeleton = _mesh->GetSkeleton();

            if (skeleton != nullptr)
                _animType = RenderableAnimType::Skinned;
            else
                _animType = RenderableAnimType::None;

            _animation->SetSkeleton(_mesh->GetSkeleton());
        }
        else
        {
            _animType = RenderableAnimType::None;
            _animation->SetSkeleton(nullptr);
        }
    }

    void Renderable::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Renderable::FrameSync()
    {
        UINT32 dirtyFlag = GetCoreDirtyFlags();
        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything | (UINT32)ActorDirtyFlag::Active;

        if ((dirtyFlag & updateEverythingFlag) != 0)
        {
            CreateAnimationBuffers();

            if (_oldActive != GetActive())
            {
                if (_active)
                {
                    if (_renderer) _renderer->NotifyRenderableAdded(this);
                }
                else
                {
                    if (_renderer) _renderer->NotifyRenderableRemoved(this);
                }
            }
            else
            {
                if (_renderer) _renderer->NotifyRenderableRemoved(this);
                if (_renderer) _renderer->NotifyRenderableAdded(this);
            }
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Mobility) != 0)
        {
            // TODO I'm not sure for that, we might check if SceneActor is active
            if (_active)
            {
                if (_renderer) _renderer->NotifyRenderableRemoved(this);
                if (_renderer) _renderer->NotifyRenderableAdded(this);
            }
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Transform) != 0)
        {
            if (_active)
            {
                if (_renderer) _renderer->NotifyRenderableUpdated(this);
            }
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::GpuParams) != 0)
        {
            CreateAnimationBuffers();

            if (_active)
            {
                if (_renderer) _renderer->NotifyRenderableUpdated(this);
            }
        }

        _oldActive = _active;
    }

    void Renderable::SetMobility(ObjectMobility mobility)
    {
        SceneActor::SetMobility(mobility);
        _markCoreDirty(ActorDirtyFlag::Mobility);
    }

    void Renderable::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        _transform = transform;
        _tfrmMatrix = transform.GetMatrix();
        _tfrmMatrixNoScale = Matrix4::TRS(transform.GetPosition(), transform.GetRotation(), Vector3::ONE);

        _boundsDirty = true;
        _subMeshesBoundsDirty = true;

        _markCoreDirty(ActorDirtyFlag::Transform);
    }

    void Renderable::SetMesh(SPtr<Mesh> mesh)
    {
        if(mesh == _mesh)
            return;

        _mesh = mesh;
        _boneMatrixBuffer = nullptr;
        _bonePrevMatrixBuffer = nullptr;

        if (_mesh)
        {
            UINT32 numSubMeshes = mesh->GetProperties().GetNumSubMeshes();
            _materials.resize(numSubMeshes);
            _subMeshesBounds.resize(numSubMeshes);
        }

        _boundsDirty = true;
        _subMeshesBoundsDirty = true;

        OnMeshChanged();
        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    void Renderable::SetMaterial(UINT32 idx, const SPtr<Material>& material)
    {
        if (!_mesh)
            return;

        if (idx >= (UINT32)_materials.size())
            return;

        _materials[idx] = material;
        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    /** Remove all the instances of this material used on submesh for this renderable */
    void Renderable::RemoveMaterial(const SPtr<Material>& material)
    {
        if (!_mesh)
            return;

        for(auto& element : _materials)
        {
            if (element == material)
            {
                element = nullptr;
                _markCoreDirty(ActorDirtyFlag::GpuParams);
            }
        }
    }

    void Renderable::SetMaterials(const Vector<SPtr<Material>>& materials)
    {
        if (!_mesh)
            return;

        _numMaterials = (UINT32)_materials.size();
        UINT32 min = std::min(_numMaterials, (UINT32)materials.size());

        for (UINT32 i = 0; i < min; i++)
        {
            _materials[i] = materials[i];
        }

        for (UINT32 i = min; i < _numMaterials; i++)
            _materials[i] = nullptr;

        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    void Renderable::SetMaterial(const SPtr<Material>& material, bool all)
    {
        if (!_mesh)
            return;

        if (!all)
        {
            SetMaterial(0, material);
        }
        else
        {
            for (UINT32 i = 0; i < _materials.size(); i++)
            {
                SetMaterial(i, material);
            }
        }
        
        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    void Renderable::SetMaterial(const String& name, const SPtr<Material>& material)
    {
        if (!_mesh)
            return;

        UINT32 numSubMeshes = _mesh->GetProperties().GetNumSubMeshes();
        UINT32 assignedSubMeshed = 0;
        
        for (UINT32 i = 0; i < numSubMeshes; i++)
        {
            const SubMesh& subMesh = _mesh->GetProperties().GetSubMesh(i);
            if (subMesh.MaterialName == name)
            {
                _materials[i] = material;
                assignedSubMeshed++;
            }
        }

        _markCoreDirty(ActorDirtyFlag::GpuParams);

        if (assignedSubMeshed == 0)
            TE_DEBUG("No submesh currently use the material {" + name + "} in {" + _mesh->GetName() + "}");
    }

    void Renderable::ClearAllMaterials()
    {
        for (UINT32 i = 0; i < (UINT32)_materials.size(); i++)
            _materials[i] = nullptr;

        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    bool Renderable::IsUsingMaterial(const SPtr<Material>& material)
    {
        for (UINT32 i = 0; i < (UINT32)_materials.size(); i++)
        {
            if(_materials[i] == material)
                return true;
        }

        return false;
    }

    void Renderable::UpdateMaterials()
    {
        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    SPtr<Material> Renderable::GetMaterial(UINT32 idx) const
    {
        if (idx >= (UINT32)_materials.size())
            return nullptr;

        return _materials[idx];
    }

    void Renderable::SetLayer(UINT64 layer)
    {
        const bool isPow2 = layer && !((layer - 1) & layer);

        if (!isPow2)
        {
            TE_DEBUG("Invalid layer provided. Only one layer bit may be set. Ignoring.");
            return;
        }

        _layer = layer;
        _markCoreDirty();
    }

    void Renderable::SetWriteVelocity(bool enable)
    {
        if (_properties.WriteVelocity == enable)
            return;

        _properties.WriteVelocity = enable;
        _markCoreDirty();
    }

    void Renderable::SetCullDistanceFactor(float factor)
    {
        _properties.CullDistanceFactor = factor;
        _markCoreDirty();
    }

    void Renderable::SetUseForDynamicEnvMapping(bool use)
    {
        _properties.UseForDynamicEnvMapping = use;
        _markCoreDirty();
    }

    void Renderable::SetAnimation(const SPtr<Animation>& animation)
    {
        _animation = animation;

        if(_animation)
            _animationId = animation->GetAnimId();
        else
            _animationId = 0;

        RefreshAnimation();
        _markCoreDirty();
    }

    Bounds Renderable::GetBounds()
    {
        SPtr<Mesh> mesh = GetMesh();

        if(!_boundsDirty)
            return _cachedBounds;

        _boundsDirty = false;

        if (mesh == nullptr)
        {
            const Transform& tfrm = GetTransform();

            AABox box(tfrm.GetPosition(), tfrm.GetPosition());
            Sphere sphere(tfrm.GetPosition(), 0.0f);

            _cachedBounds = Bounds(box, sphere);
            return _cachedBounds;
        }
        else
        {
            _cachedBounds = mesh->GetProperties().GetBounds();
            _cachedBounds.TransformAffine(_tfrmMatrix);
            return _cachedBounds;
        }
    }

    Bounds Renderable::GetSubMeshBounds(UINT32 subMeshIdx)
    {
        SPtr<Mesh> mesh = GetMesh();

        if (!mesh || subMeshIdx > mesh->GetProperties().GetNumSubMeshes() - 1)
        {
            const Transform& tfrm = GetTransform();

            AABox box(tfrm.GetPosition(), tfrm.GetPosition());
            Sphere sphere(tfrm.GetPosition(), 0.0f);

            return Bounds(box, sphere);
        }

        if (!_subMeshesBoundsDirty)
            return _subMeshesBounds.at(subMeshIdx);

        Bounds returnBounds;

        _subMeshesBoundsDirty = false;
        _subMeshesBounds.clear();

        for (UINT32 i = 0; i < mesh->GetProperties().GetNumSubMeshes(); i++)
        {
            SubMesh* subMesh = mesh->GetProperties().GetSubMeshPtr(i);
            if (subMesh)
            {
                Bounds subMeshBounds = subMesh->SubMeshBounds;
                subMeshBounds.TransformAffine(_tfrmMatrix);

                _subMeshesBounds.push_back(subMeshBounds);

                if (i == subMeshIdx)
                    returnBounds = subMeshBounds;
            }
            else
            {
                _subMeshesBounds.push_back(returnBounds);
            }
        }

        return returnBounds;
    }

    void Renderable::UpdateState(const SceneObject& so, bool force)
    {
        UINT32 curHash = so.GetTransformHash();
        if (curHash != _hash || force)
        {
            // If skinned animation, don't include own transform since that will be handled by root bone animation
            bool ignoreOwnTransform;
            if (_animType == RenderableAnimType::Skinned)
                ignoreOwnTransform = _animation->GetAnimatesRoot();
            else
                ignoreOwnTransform = false;

            if (ignoreOwnTransform)
            {
                // Note: Technically we're checking child's hash but using parent's transform. Ideally we check the parent's
                // hash to reduce the number of required updates.
                HSceneObject parentSO = so.GetParent();
                if (parentSO != nullptr)
                    SetTransform(parentSO->GetTransform());
                else
                    SetTransform(Transform());
            }
            else
            {
                SetTransform(so.GetTransform());
            }

            _hash = curHash;
        }

        // Hash now matches so transform won't be applied twice, so we can just call base class version
        SceneActor::UpdateState(so, force);
    }

    void Renderable::CreateAnimationBuffers()
    {
        if (_animType == RenderableAnimType::Skinned)
        {
            SPtr<Skeleton> skeleton = _mesh->GetSkeleton();
            UINT32 numBones = skeleton != nullptr ? skeleton->GetNumBones() : 0;

            if (numBones > 0)
            {
                // If matrix buffer already exists, we don't want to destory it
                if(!_boneMatrixBuffer) 
                {
                    _boneMatrixBuffer = CreateBoneMatrixBuffer(numBones);

                    if (_properties.WriteVelocity)
                        _bonePrevMatrixBuffer = CreateBoneMatrixBuffer(numBones);
                    else
                        _bonePrevMatrixBuffer = nullptr;
                }
            }
            else
            {
                _boneMatrixBuffer = nullptr;
                _bonePrevMatrixBuffer = nullptr;
            }
        }
        else
        {
            _boneMatrixBuffer = nullptr;
            _bonePrevMatrixBuffer = nullptr;
        }
    }

    void Renderable::UpdateAnimationBuffers(const EvaluatedAnimationData& animData)
    {
        if (_animationId == (UINT64)-1)
            return;

        const EvaluatedAnimationData::AnimInfo* animInfo = nullptr;

        auto iterFind = animData.Infos.find(_animationId);
        if (iterFind != animData.Infos.end())
            animInfo = &iterFind->second;

        if (animInfo == nullptr)
            return;

        if (_animType == RenderableAnimType::Skinned)
        {
            AnimationManager::Instance().SetAnimDataDirty();
            const EvaluatedAnimationData::PoseInfo& poseInfo = animInfo->PoseInfos;

            if (_properties.WriteVelocity)
                std::swap(_boneMatrixBuffer, _bonePrevMatrixBuffer);

            // Note: If multiple elements are using the same animation (not possible atm), this buffer should be shared by
            // all such elements
            UINT8* dest = (UINT8*)_boneMatrixBuffer->Lock(0, poseInfo.NumBones * 4 * sizeof(Vector4), GBL_WRITE_ONLY_DISCARD);
            for (UINT32 j = 0; j < poseInfo.NumBones; j++)
            {
                const Matrix4& transform = animData.Transforms[poseInfo.StartIdx + j];
                memcpy(dest, &transform, 16 * sizeof(float)); // Assuming row-major format

                dest += 16 * sizeof(float);
            }

            _boneMatrixBuffer->Unlock();
        }
    }

    void Renderable::UpdatePrevFrameAnimationBuffers()
    {
        if (_animType == RenderableAnimType::Skinned)
            std::swap(_boneMatrixBuffer, _bonePrevMatrixBuffer);
    }

    void Renderable::AttachTo(SPtr<Renderer> renderer)
    {
        if (_renderer)
            _renderer->NotifyRenderableRemoved(this);

        _renderer = renderer;

        if (_renderer)
            _renderer->NotifyRenderableAdded(this);

        _markCoreDirty();
    }

    SPtr<Renderable> Renderable::Create()
    {
        SPtr<Renderable> handlerPtr = CreateEmpty();
        handlerPtr->Initialize();

        return handlerPtr;
    }

    SPtr<Renderable> Renderable::CreateEmpty()
    {
        Renderable* handler = new (te_allocate<Renderable>()) Renderable();
        SPtr<Renderable> handlerPtr = te_core_ptr<Renderable>(handler);
        handlerPtr->SetThisPtr(handlerPtr);

        return handlerPtr;
    }
}
