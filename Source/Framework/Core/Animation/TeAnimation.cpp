#include "TeAnimation.h"

namespace te
{
    AnimationClipInfo::AnimationClipInfo(const HAnimationClip& clip)
        : Clip(clip)
    { }

    AnimationProxy::AnimationProxy(UINT64 id)
        : Id(id)
    { }

    AnimationProxy::~AnimationProxy()
    {
        Clear();
    }

    void AnimationProxy::Clear()
    {
        // TODO animation
    }

    Animation::Animation()
        : Serializable(TID_Animation)
    {
        _animId = AnimationManager::Instance().RegisterAnimation(this);
        _animProxy = te_shared_ptr_new<AnimationProxy>(_animId);
    }

    Animation::~Animation()
    {
        AnimationManager::Instance().UnregisterAnimation(_animId);
    }

    void Animation::SetSkeleton(const SPtr<Skeleton>& skeleton)
    {
        _skeleton = skeleton;
        _dirty |= (UINT32)AnimDirtyStateFlag::All;
    }

    void Animation::SetMask(const SkeletonMask& mask)
    {
        _skeletonMask = mask;
        _dirty |= (UINT32)AnimDirtyStateFlag::All;
    }

    void Animation::SetWrapMode(AnimWrapMode wrapMode)
    {
        _defaultWrapMode = wrapMode;

        for (auto& clipInfo : _clipInfos)
            clipInfo.State.WrapMode = wrapMode;

        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
    }

    void Animation::SetSpeed(float speed)
    {
        _defaultSpeed = speed;

        for (auto& clipInfo : _clipInfos)
        {
            // Special case: Ignore non-moving ones
            if (!clipInfo.State.Stopped)
                clipInfo.State.Speed = speed;
        }

        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
    }

    void Animation::SetBounds(const AABox& bounds)
    {
        _bounds = bounds;
        _dirty |= (UINT32)AnimDirtyStateFlag::Culling;
    }

    void Animation::SetCulling(bool cull)
    {
        _cull = cull;
        _dirty |= (UINT32)AnimDirtyStateFlag::Culling;
    }

    void Animation::Play(const HAnimationClip& clip)
    {
        AnimationClipInfo* clipInfo = AddClip(clip, (UINT32)-1);
        if (clipInfo != nullptr)
        {
            clipInfo->State.Time = 0.0f;
            clipInfo->State.Speed = _defaultSpeed;
            clipInfo->State.Weight = 1.0f;
            clipInfo->State.WrapMode = _defaultWrapMode;
            clipInfo->PlaybackType = AnimPlaybackType::Normal;
        }

        _sampleStep = AnimSampleStep::None;
        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
    }

    void Animation::Blend1D(const Blend1DInfo& info, float t)
    {
        // TODO animation
    }

    void Animation::Blend2D(const Blend2DInfo& info, const Vector2& t)
    {
        // TODO animation
    }

    void Animation::CrossFade(const HAnimationClip& clip, float fadeLength)
    {
        // TODO animation
    }

    void Animation::Sample(const HAnimationClip& clip, float time)
    {
        // TODO animation
    }

    void Animation::Stop(UINT32 layer)
    {
        // TODO animation
    }

    void Animation::StopAll()
    {
        // TODO animation
    }

    AnimationClipInfo* Animation::AddClip(const HAnimationClip& clip, UINT32 layer, bool stopExisting)
    {
        AnimationClipInfo* output = nullptr;
        return output;

        // TODO animation
    }

    bool Animation::GetAnimatesRoot() const
    {
        return false;

        // TODO animation
    }

    bool Animation::IsPlaying() const
    {
        return false;

        // TODO animation
    }

    bool Animation::GetState(const HAnimationClip& clip, AnimationClipState& state)
    {
        return false;
        // TODO animation
    }

    void Animation::SetState(const HAnimationClip& clip, AnimationClipState state)
    {
        // TODO animation
    }

    UINT32 Animation::GetNumClips() const
    {
        return (UINT32)_clipInfos.size();
    }

    HAnimationClip Animation::GetClip(UINT32 idx) const
    {
        if (idx >= (UINT32)_clipInfos.size())
            return HAnimationClip();

        return _clipInfos[idx].Clip;
    }

    void Animation::TriggerEvents(float delta)
    {
        // TODO animation
    }

    void Animation::MapCurveToSceneObject(const String& curve, const HSceneObject& so)
    {
        AnimatedSceneObject animSo = { so, curve };
        _sceneObjects[so.GetInstanceId()] = animSo;

        _dirty |= (UINT32)AnimDirtyStateFlag::All;
    }

    void Animation::UnmapSceneObject(const HSceneObject& so)
    {
        _sceneObjects.erase(so.GetInstanceId());

        _dirty |= (UINT32)AnimDirtyStateFlag::All;
    }

    bool Animation::GetGenericCurveValue(UINT32 curveIdx, float& value)
    {
        if (!_genericCurveValuesValid || curveIdx >= (UINT32)_genericCurveOutputs.size())
            return false;

        value = _genericCurveOutputs[curveIdx];
        return true;
    }

    SPtr<Animation> Animation::Create()
    {
        Animation* anim = new (te_allocate<Animation>()) Animation();

        SPtr<Animation> animPtr = te_core_ptr(anim);
        animPtr->SetThisPtr(animPtr);
        animPtr->Initialize();

        return animPtr;
    }

    void Animation::UpdateAnimProxy(float timeDelta)
    {
        // TODO animation
    }

    void Animation::UpdateFromProxy()
    {
        // TODO animation
    }
}
