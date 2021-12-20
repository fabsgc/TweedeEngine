#include "Components/TeCAnimation.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCBone.h"
#include "TeCoreApplication.h"
#include "Mesh/TeMesh.h"

using namespace std::placeholders;

namespace te
{
    CAnimation::CAnimation()
        : Component(HSceneObject(), (UINT32)TID_CAnimation)
    {
        _notifyFlags = TCF_Transform;

        SetName("Animation");
        SetFlag(Component::AlwaysRun, true);
    }

    CAnimation::CAnimation(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CAnimation)
    {
        _notifyFlags = TCF_Transform;

        SetName("Animation");
        SetFlag(Component::AlwaysRun, true);
    }

    void CAnimation::SetDefaultClip(const HAnimationClip& clip)
    {
        _defaultClip = clip;

        if (_internal != nullptr && _defaultClip.IsLoaded())
            _internal->Play(clip);
    }

    void CAnimation::SetWrapMode(AnimWrapMode wrapMode)
    {
        _wrapMode = wrapMode;

        if (_internal != nullptr && !_previewMode)
            _internal->SetWrapMode(wrapMode);
    }

    void CAnimation::SetSpeed(float speed)
    {
        _speed = speed;

        if (_internal != nullptr && !_previewMode)
            _internal->SetSpeed(speed);
    }

    void CAnimation::Play(const HAnimationClip& clip)
    {
        if (_internal != nullptr && !_previewMode)
            _internal->Play(clip);
    }

    void CAnimation::BlendAdditive(const HAnimationClip& clip, float weight, float fadeLength, UINT32 layer)
    {
        if (_internal != nullptr && !_previewMode)
            _internal->BlendAdditive(clip, weight, fadeLength, layer);
    }

    void CAnimation::Blend1D(const Blend1DInfo& info, float t)
    {
        if (_internal != nullptr && !_previewMode)
            _internal->Blend1D(info, t);
    }

    void CAnimation::Blend2D(const Blend2DInfo& info, const Vector2& t)
    {
        if (_internal != nullptr && !_previewMode)
            _internal->Blend2D(info, t);
    }

    void CAnimation::CrossFade(const HAnimationClip& clip, float fadeLength)
    {
        if (_internal != nullptr && !_previewMode)
            _internal->CrossFade(clip, fadeLength);
    }

    void CAnimation::Sample(const HAnimationClip& clip, float time)
    {
        if (_internal != nullptr)
            _internal->Sample(clip, time);
    }

    void CAnimation::Stop(UINT32 layer)
    {
        if (_internal != nullptr && !_previewMode)
            _internal->Stop(layer);
    }

    void CAnimation::StopAll()
    {
        if (_internal != nullptr && !_previewMode)
            _internal->StopAll();
    }

    bool CAnimation::IsPlaying() const
    {
        if (_internal != nullptr)
            return _internal->IsPlaying();

        return false;
    }

    bool CAnimation::GetState(const HAnimationClip& clip, AnimationClipState& state)
    {
        if (_internal != nullptr)
            return _internal->GetState(clip, state);

        return false;
    }

    void CAnimation::SetState(const HAnimationClip& clip, AnimationClipState state)
    {
        if (_internal != nullptr)
            return _internal->SetState(clip, state);
    }

    void CAnimation::SetEnableCull(bool enable)
    {
        _enableCull = enable;

        if (_internal != nullptr && !_previewMode)
            _internal->SetCulling(enable);
    }

    UINT32 CAnimation::GetNumClips() const
    {
        if (_internal != nullptr)
            return _internal->GetNumClips();

        return 0;
    }

    HAnimationClip CAnimation::GetClip(UINT32 idx) const
    {
        if (_internal != nullptr)
            return _internal->GetClip(idx);

        return HAnimationClip();
    }

    void CAnimation::Instantiate()
    { }

    void CAnimation::Initialize()
    {
        RestoreInternal(false);
        Component::Initialize();
    }

    void CAnimation::OnInitialized()
    {
        Component::OnInitialized();
    }

    void CAnimation::OnEnabled()
    {
        if (_previewMode)
        {
            DestroyInternal();
            _previewMode = false;
        }

        RestoreInternal(false);
        Component::OnEnabled();
    }

    void CAnimation::OnDisabled()
    {
        DestroyInternal();
        Component::OnDisabled();
    }

    void CAnimation::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & (TCF_Transform)) != 0)
            UpdateBounds();
    }

    void CAnimation::OnDestroyed()
    {
        DestroyInternal();
        Component::OnDestroyed();
    }

    void CAnimation::RestoreInternal(bool previewMode)
    {
        if (_internal != nullptr)
            DestroyInternal();

        _internal = Animation::Create();

        _animatedRenderable = static_object_cast<CRenderable>(SO()->GetComponent<CRenderable>());

        if (_animatedRenderable)
        {
            _internal->SetBounds(_animatedRenderable->GetBounds().GetBox());
        }

        if (!previewMode)
        {
            _internal->OnEventTriggered.Connect(std::bind(&CAnimation::EventTriggered, this, _1, _2));

            _internal->SetWrapMode(_wrapMode);
            _internal->SetSpeed(_speed);
            _internal->SetCulling(_enableCull);
        }

        if (!previewMode)
        {
            if(_defaultClip.IsLoaded())
                _internal->Play(_defaultClip);

            _primaryPlayingClip = _internal->GetClip(0);
            if (_primaryPlayingClip.IsLoaded())
            {
                // TODO animation script
            }
        }

        SetBoneMappings();

        if (!previewMode)
            UpdateSceneObjectMapping();

        if (_animatedRenderable != nullptr)
            _animatedRenderable->RegisterAnimation(static_object_cast<CAnimation>(_thisHandle));
    }

    void CAnimation::DestroyInternal()
    {
        if (_animatedRenderable != nullptr)
            _animatedRenderable->UnregisterAnimation();

        _primaryPlayingClip = nullptr;

        // This should release the last reference and destroy the internal listener
        _internal = nullptr;
    }

    void CAnimation::Update()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Game);
        if (!isRunning && !_previewMode)
        {
            // Make sure attached CBone components match the position of the skeleton bones even when the component is not
            // otherwise running.

            HRenderable animatedRenderable = static_object_cast<CRenderable>(SO()->GetComponent<CRenderable>());
            if (animatedRenderable)
            {
                SPtr<Mesh> mesh = animatedRenderable->GetMesh();
                if (mesh)
                {
                    const SPtr<Skeleton>& skeleton = mesh->GetSkeleton();
                    if (skeleton)
                    {
                        for (auto& entry : _mappingInfos)
                        {
                            // We allow a null bone for the root bone mapping, should be non-null for everything else
                            if (!entry.IsMappedToBone || entry.Bone == nullptr)
                                continue;

                            const UINT32 numBones = skeleton->GetNumBones();
                            for (UINT32 j = 0; j < numBones; j++)
                            {
                                if (skeleton->GetBoneInfo(j).Name == entry.Bone->GetBoneName())
                                {
                                    Matrix4 bindPose = skeleton->GetInvBindPose(j).InverseAffine();
                                    bindPose = SO()->GetTransform().GetMatrix() * bindPose;

                                    Vector3 position, scale;
                                    Quaternion rotation;
                                    bindPose.Decomposition(position, rotation, scale);

                                    entry.So->SetWorldPosition(position);
                                    entry.So->SetWorldRotation(rotation);
                                    entry.So->SetWorldScale(scale);

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (_internal == nullptr || !isRunning)
            return;

        HAnimationClip newPrimaryClip = _internal->GetClip(0);
        if (newPrimaryClip != _primaryPlayingClip)
            RefreshClipMappings();

        // TODO animation : script
    }

    bool CAnimation::GetGenericCurveValue(UINT32 curveIdx, float& value)
    {
        if (_internal == nullptr)
            return false;

        return _internal->GetGenericCurveValue(curveIdx, value);
    }

    void CAnimation::UpdateBounds()
    {
        if (_internal != nullptr)
        {
            AABox bounds;
            if (_animatedRenderable != nullptr)
                bounds = _animatedRenderable->GetBounds().GetBox();

            _internal->SetBounds(bounds);
        }
    }

    void CAnimation::MapCurveToSceneObject(const String& curve, const HSceneObject& so)
    {
        if (_internal == nullptr)
            return;

        _internal->MapCurveToSceneObject(curve, so);
    }

    void CAnimation::UnmapSceneObject(const HSceneObject& so)
    {
        if (_internal == nullptr)
            return;

        _internal->UnmapSceneObject(so);
    }

    void CAnimation::AddBone(HBone bone)
    {
        const HSceneObject& currentSO = bone->SO();

        SceneObjectMappingInfo newMapping;
        newMapping.So = currentSO;
        newMapping.IsMappedToBone = true;
        newMapping.Bone = std::move(bone);

        _mappingInfos.push_back(newMapping);

        if (_internal)
            _internal->MapCurveToSceneObject(newMapping.Bone->GetBoneName(), newMapping.So);
    }

    void CAnimation::RemoveBone(const HBone& bone)
    {
        HSceneObject newSO;
        for (UINT32 i = 0; i < (UINT32)_mappingInfos.size(); i++)
        {
            if (_mappingInfos[i].Bone == bone)
            {
                if (_internal)
                    _internal->UnmapSceneObject(_mappingInfos[i].So);

                _mappingInfos.erase(_mappingInfos.begin() + i);
                i--;
            }
        }
    }

    void CAnimation::NotifyBoneChanged(const HBone& bone)
    {
        if (_internal == nullptr)
            return;

        for (UINT32 i = 0; i < (UINT32)_mappingInfos.size(); i++)
        {
            if (_mappingInfos[i].Bone == bone)
            {
                _internal->UnmapSceneObject(_mappingInfos[i].So);
                _internal->MapCurveToSceneObject(bone->GetBoneName(), _mappingInfos[i].So);
                break;
            }
        }
    }

    void CAnimation::RegisterRenderable(const HRenderable& renderable)
    {
        _animatedRenderable = renderable;
    }

    void CAnimation::UnregisterRenderable()
    {
        _animatedRenderable = nullptr;
    }

    void CAnimation::SetBoneMappings()
    {
        _mappingInfos.clear();

        SceneObjectMappingInfo rootMapping;
        rootMapping.So = SO();
        rootMapping.IsMappedToBone = true;

        _mappingInfos.push_back(rootMapping);
        _internal->MapCurveToSceneObject("", rootMapping.So);

        Vector<HBone> childBones = FindChildBones();
        for (auto& entry : childBones)
            AddBone(entry);
    }

    void CAnimation::UpdateSceneObjectMapping()
    {
        Vector<SceneObjectMappingInfo> newMappingInfos;
        for (auto& entry : _mappingInfos)
        {
            if (entry.IsMappedToBone)
                newMappingInfos.push_back(entry);
            else
                UnmapSceneObject(entry.So);
        }

        if (_primaryPlayingClip.IsLoaded())
        {
            HSceneObject root = SO();

            const auto& findMappings = [&](const String& name, AnimationCurveFlags flags)
            {
                if (flags |= (UINT32)AnimationCurveFlag::ImportedCurve)
                    return;

                // TODO animation don't know if it works
                HSceneObject currentSO = root->GetSceneObject(name, true); 

                bool found = false;
                for (UINT32 i = 0; i < (UINT32)newMappingInfos.size(); i++)
                {
                    if (newMappingInfos[i].So == currentSO)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    SceneObjectMappingInfo newMappingInfo;
                    newMappingInfo.IsMappedToBone = false;
                    newMappingInfo.So= currentSO;

                    newMappingInfos.push_back(newMappingInfo);
                    MapCurveToSceneObject(name, currentSO);
                }
            };

            SPtr<AnimationCurves> curves = _primaryPlayingClip->GetCurves();
            for (auto& curve : curves->Position)
                findMappings(curve.Name, curve.Flags);

            for (auto& curve : curves->Rotation)
                findMappings(curve.Name, curve.Flags);

            for (auto& curve : curves->Scale)
                findMappings(curve.Name, curve.Flags);
        }

        _mappingInfos = newMappingInfos;
    }

    void CAnimation::RefreshClipMappings()
    {
        _primaryPlayingClip = _internal->GetClip(0);

        // TODO animation script

        UpdateSceneObjectMapping();
    }

    Vector<HBone> CAnimation::FindChildBones()
    {
        Stack<HSceneObject> todo;
        todo.push(SO());

        Vector<HBone> bones;
        while (todo.size() > 0)
        {
            HSceneObject currentSO = todo.top();
            todo.pop();

            HBone bone = static_object_cast<CBone>(currentSO->GetComponent<CBone>());
            if (bone != nullptr)
            {
                bone->SetParent(static_object_cast<CAnimation>(GetHandle()), true);
                bones.push_back(bone);
            }

            int childCount = currentSO->GetNumChildren();
            for (int i = 0; i < childCount; i++)
            {
                HSceneObject child = currentSO->GetChild(i);
                if (!child->GetComponent<CAnimation>().Empty())
                    continue;

                todo.push(child);
            }
        }

        return bones;
    }

    void CAnimation::EventTriggered(const HAnimationClip& clip, const String& name)
    {
        OnEventTriggered(clip, name);

        // TODO animation script
    }

    void CAnimation::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CAnimation>(c));
    }

    void CAnimation::Clone(const HAnimation& c)
    { 
        Component::Clone(c.GetInternalPtr());

        _defaultClip = c->_defaultClip.GetNewHandleFromExisting();
        _primaryPlayingClip = c->_primaryPlayingClip.GetNewHandleFromExisting();
        _wrapMode = c->_wrapMode;
        _speed = c->_speed;
        _enableCull = c->_enableCull;
        _previewMode = c->_previewMode;
    }
}
