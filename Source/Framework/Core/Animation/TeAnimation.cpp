#include "TeAnimation.h"

#include "Scene/TeSceneObject.h"
#include "Animation/TeAnimationUtility.h"

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
        if (_layers == nullptr)
            return;

        for (UINT32 i = 0; i < _numLayers; i++)
        {
            AnimationStateLayer& layer = _layers[i];
            for (UINT32 j = 0; j < layer.NumStates; j++)
            {
                AnimationState& state = layer.States[j];

                if (_skeleton != nullptr)
                {
                    UINT32 numBones = _skeleton->GetNumBones();
                    for (UINT32 k = 0; k < numBones; k++)
                        state.BoneToCurveMapping[k].~AnimationCurveMapping();
                }

                if (state.SoToCurveMapping != nullptr)
                {
                    for (UINT32 k = 0; k < _numSceneObjects; k++)
                        state.SoToCurveMapping[k].~AnimationCurveMapping();
                }

                state.~AnimationState();
            }

            layer.~AnimationStateLayer();
        }

        // All of the memory is part of the same buffer, so we only need to free the first element
        te_free(_layers);
        _layers = nullptr;
        _genericCurveOutputs = nullptr;
        _sceneObjectInfos = nullptr;
        _sceneObjectTransforms = nullptr;

        _numLayers = 0;
        _numGenericCurves = 0;
    }

    void AnimationProxy::Rebuild(const SPtr<Skeleton>& skeleton, const SkeletonMask& mask,
        Vector<AnimationClipInfo>& clipInfos, const Vector<AnimatedSceneObject>& sceneObjects)
    {
        _skeleton = skeleton;
        _skeletonMask = mask;

        if (skeleton != nullptr)
            _skeletonPose = LocalSkeletonPose(skeleton->GetNumBones());

        _numSceneObjects = (UINT32)sceneObjects.size();
        if (_numSceneObjects > 0)
            _sceneObjectPose = LocalSkeletonPose(_numSceneObjects, true);
        else
            _sceneObjectPose = LocalSkeletonPose();

        Rebuild(clipInfos, sceneObjects);
    }

    void AnimationProxy::Rebuild(Vector<AnimationClipInfo>& clipInfos, const Vector<AnimatedSceneObject>& sceneObjects)
    {
        Clear();

        Vector<bool> clipLoadState(clipInfos.size());
        Vector<AnimationStateLayer> tempLayers;
        UINT32 clipIdx = 0;
        for (auto& clipInfo : clipInfos)
        {
            UINT32 layer = clipInfo.State.Layer;
            if (layer == (UINT32)-1)
                layer = 0;
            else
                layer += 1;

            auto iterFind = std::find_if(tempLayers.begin(), tempLayers.end(),
                [&](auto& x)
                {
                    return x.Index == layer;
                }
            );

            bool isLoaded = clipInfo.Clip.GetHandleData() != nullptr;
            clipLoadState[clipIdx] = isLoaded;

            if (iterFind == tempLayers.end())
            {
                tempLayers.push_back(AnimationStateLayer());
                AnimationStateLayer& newLayer = tempLayers.back();

                newLayer.Index = layer;
            }

            clipIdx++;
        }

        std::sort(tempLayers.begin(), tempLayers.end(),
            [&](auto& x, auto& y)
            {
                return x.Index < y.Index;
            }
        );

        _numLayers = (UINT32)tempLayers.size();
        UINT32 numClips = (UINT32)clipInfos.size();
        UINT32 numBones;

        if (_skeleton != nullptr)
            numBones = _skeleton->GetNumBones();
        else
            numBones = 0;

        UINT32 numPosCurves = 0;
        UINT32 numRotCurves = 0;
        UINT32 numScaleCurves = 0;

        clipIdx = 0;
        for (auto& clipInfo : clipInfos)
        {
            bool isLoaded = clipLoadState[clipIdx++];
            if (!isLoaded)
                continue;

            SPtr<AnimationCurves> curves = clipInfo.Clip->GetCurves();
            numPosCurves += (UINT32)curves->Position.size();
            numRotCurves += (UINT32)curves->Rotation.size();
            numScaleCurves += (UINT32)curves->Scale.size();
        }

        _numGenericCurves = 0;
        if (clipInfos.size() > 0 && clipLoadState[0])
        {
            SPtr<AnimationCurves> curves = clipInfos[0].Clip->GetCurves();
            _numGenericCurves = (UINT32)curves->Generic.size();
        }

        UINT32* mappedBoneIndices = (UINT32*)te_newN<UINT32>(sizeof(UINT32) * _numSceneObjects);
        for (UINT32 i = 0; i < _numSceneObjects; i++)
            mappedBoneIndices[i] = -1;

        UINT32 numBoneMappedSOs = 0;
        if (_skeleton != nullptr)
        {
            for (UINT32 i = 0; i < _numSceneObjects; i++)
            {
                if (sceneObjects[i].So.IsDestroyed(true))
                    continue;

                // Empty string always means root bone
                if (sceneObjects[i].CurveName.empty())
                {
                    UINT32 rootBoneIdx = _skeleton->GetRootBoneIndex();
                    if (rootBoneIdx != (UINT32)-1)
                    {
                        mappedBoneIndices[i] = rootBoneIdx;
                        numBoneMappedSOs++;
                    }
                }
                else
                {
                    for (UINT32 j = 0; j < numBones; j++)
                    {
                        if (_skeleton->GetBoneInfo(j).Name == sceneObjects[i].CurveName)
                        {
                            mappedBoneIndices[i] = j;

                            numBoneMappedSOs++;
                            break;
                        }
                    }
                }
            }
        }

        UINT32 numBoneMappings = numBones * numClips;
        UINT32 layersSize = sizeof(AnimationStateLayer) * _numLayers;
        UINT32 clipsSize = sizeof(AnimationState) * numClips;
        UINT32 boneMappingSize = numBoneMappings * sizeof(AnimationCurveMapping);
        UINT32 genericCurveOutputSize = _numGenericCurves * sizeof(float);
        UINT32 sceneObjectIdsSize = _numSceneObjects * sizeof(AnimatedSceneObjectInfo);
        UINT32 sceneObjectTransformsSize = numBoneMappedSOs * sizeof(Matrix4);

        UINT8* data = (UINT8*)te_allocate(layersSize + clipsSize + boneMappingSize + genericCurveOutputSize + sceneObjectIdsSize + sceneObjectTransformsSize);

        _layers = (AnimationStateLayer*)data;
        memcpy(_layers, tempLayers.data(), layersSize);
        data += layersSize;

        AnimationState* states = (AnimationState*)data;
        for (UINT32 i = 0; i < numClips; i++)
            new (&states[i]) AnimationState();

        data += clipsSize;

        AnimationCurveMapping* boneMappings = (AnimationCurveMapping*)data;
        for (UINT32 i = 0; i < numBoneMappings; i++)
            new (&boneMappings[i]) AnimationCurveMapping();

        data += boneMappingSize;

        _genericCurveOutputs = (float*)data;
        data += genericCurveOutputSize;

        _sceneObjectInfos = (AnimatedSceneObjectInfo*)data;
        data += sceneObjectIdsSize;

        _sceneObjectTransforms = (Matrix4*)data;
        for (UINT32 i = 0; i < numBoneMappedSOs; i++)
            _sceneObjectTransforms[i] = Matrix4::IDENTITY;

        data += sceneObjectTransformsSize;

        UINT32 curLayerIdx = 0;
        UINT32 curStateIdx = 0;

        // Note: Hidden dependency. First clip info must be in layers[0].states[0] (needed for generic curves which only
        // use the primary clip).
        for (UINT32 i = 0; i < _numLayers; i++)
        {
            AnimationStateLayer& layer = _layers[i];

            layer.States = &states[curStateIdx];
            layer.NumStates = 0;

            UINT32 localStateIdx = 0;
            for (UINT32 j = 0; j < (UINT32)clipInfos.size(); j++)
            {
                AnimationClipInfo& clipInfo = clipInfos[j];

                UINT32 clipLayer = clipInfo.State.Layer;
                if (clipLayer == (UINT32)-1)
                    clipLayer = 0;
                else
                    clipLayer += 1;

                if (clipLayer != layer.Index)
                    continue;

                AnimationState& state = states[curStateIdx];
                state.Loop = clipInfo.State.WrapMode == AnimWrapMode::Loop;

                // Calculate weight if fading is active
                float weight = clipInfo.State.Weight;

                //// Assumes time is clamped to [0, fadeLength] and fadeLength != 0
                if (clipInfo.FadeDirection < 0.0f)
                {
                    float t = clipInfo.FadeTime / clipInfo.FadeLength;
                    weight *= (1.0f - t);
                }
                else if (clipInfo.FadeDirection > 0.0f)
                {
                    float t = clipInfo.FadeTime / clipInfo.FadeLength;
                    weight *= t;
                }

                state.Weight = weight;

                // Set up individual curves and their caches
                bool isClipValid = clipLoadState[j];
                if (isClipValid)
                {
                    state.Curves = clipInfo.Clip->GetCurves();
                    state.Length = clipInfo.Clip->GetLength();
                    state.Disabled = clipInfo.PlaybackType == AnimPlaybackType::None;
                }
                else
                {
                    static SPtr<AnimationCurves> zeroCurves = te_shared_ptr_new<AnimationCurves>();
                    state.Curves = zeroCurves;
                    state.Length = 0.0f;
                    state.Disabled = true;
                }

                // Wrap time if looping
                if (state.Loop && state.Length > 0.0f)
                    state.Time = Math::Repeat(clipInfo.State.Time, state.Length);
                else
                    state.Time = clipInfo.State.Time;

                clipInfo.LayerIdx = curLayerIdx;
                clipInfo.StateIdx = localStateIdx;

                // TODO animation : curve version

                // Set up bone mapping
                if (_skeleton != nullptr)
                {
                    state.BoneToCurveMapping = &boneMappings[curStateIdx * numBones];

                    if (isClipValid)
                    {
                        clipInfo.Clip->GetBoneMapping(*_skeleton, state.BoneToCurveMapping);
                    }
                    else
                    {
                        AnimationCurveMapping emptyMapping = { (UINT32)-1, (UINT32)-1, (UINT32)-1 };

                        for (UINT32 i = 0; i < numBones; i++)
                            state.BoneToCurveMapping[i] = emptyMapping;
                    }
                }
                else
                    state.BoneToCurveMapping = nullptr;

                layer.NumStates++;
                curStateIdx++;
                localStateIdx++;
            }

            curLayerIdx++;

            // Must be larger than zero otherwise the layer.states pointer will point to data held by some other layer
            assert(layer.NumStates > 0);
        }

        Matrix4 invRootTransform(TeIdentity);
        for (UINT32 i = 0; i < _numSceneObjects; i++)
        {
            if (sceneObjects[i].CurveName.empty())
            {
                HSceneObject so = sceneObjects[i].So;
                if (!so.IsDestroyed(true))
                    invRootTransform = so->GetWorldMatrix().InverseAffine();

                break;
            }
        }

        UINT32 boneIdx = 0;
        for (UINT32 i = 0; i < _numSceneObjects; i++)
        {
            HSceneObject so = sceneObjects[i].So;
            AnimatedSceneObjectInfo& soInfo = _sceneObjectInfos[i];
            soInfo.Id = so.GetInstanceId();
            soInfo.BoneIdx = mappedBoneIndices[i];

            bool isSOValid = !so.IsDestroyed(true);
            if (isSOValid)
                soInfo.Hash = so->GetTransformHash();
            else
                soInfo.Hash = 0;

            soInfo.LayerIdx = (UINT32)-1;
            soInfo.StateIdx = (UINT32)-1;

            // If no bone mapping, find curves directly
            if (soInfo.BoneIdx == -1)
            {
                soInfo.CurveIndices = { (UINT32)-1, (UINT32)-1, (UINT32)-1 };

                if (isSOValid)
                {
                    for (UINT32 j = 0; j < (UINT32)clipInfos.size(); j++)
                    {
                        AnimationClipInfo& clipInfo = clipInfos[j];

                        soInfo.LayerIdx = clipInfo.LayerIdx;
                        soInfo.StateIdx = clipInfo.StateIdx;

                        bool isClipValid = clipLoadState[j];
                        if (isClipValid)
                        {
                            // Note: If there are multiple clips with the relevant curve name, we only use the first

                            clipInfo.Clip->GetCurveMapping(sceneObjects[i].CurveName, soInfo.CurveIndices);
                            break;
                        }
                    }
                }
            }
            else
            {
                // No need to check if SO is valid, if it has a bone connection it must be
                _sceneObjectTransforms[boneIdx] = so->GetWorldMatrix() * invRootTransform;
                boneIdx++;
            }
        }

        te_deleteN(mappedBoneIndices, _numSceneObjects);
    }

    void AnimationProxy::UpdateClipInfos(const Vector<AnimationClipInfo>& clipInfos)
    {
        for (auto& clipInfo : clipInfos)
        {
            AnimationState& state = _layers[clipInfo.LayerIdx].States[clipInfo.StateIdx];

            state.Loop = clipInfo.State.WrapMode == AnimWrapMode::Loop;
            state.Weight = clipInfo.State.Weight;

            // Wrap time if looping
            if (state.Loop && state.Length > 0.0f)
                state.Time = Math::Repeat(clipInfo.State.Time, state.Length);
            else
                state.Time = clipInfo.State.Time;

            state.Disabled = (clipInfo.PlaybackType == AnimPlaybackType::None);
        }
    }

    void AnimationProxy::UpdateTransforms(const Vector<AnimatedSceneObject>& sceneObjects)
    {
        Matrix4 invRootTransform(TeIdentity);
        for (UINT32 i = 0; i < _numSceneObjects; i++)
        {
            if (sceneObjects[i].CurveName.empty())
            {
                HSceneObject so = sceneObjects[i].So;
                if (!so.IsDestroyed(true))
                    invRootTransform = so->GetWorldMatrix().InverseAffine();

                break;
            }
        }

        UINT32 boneIdx = 0;
        for (UINT32 i = 0; i < _numSceneObjects; i++)
        {
            HSceneObject so = sceneObjects[i].So;
            if (so.IsDestroyed(true))
            {
                _sceneObjectInfos[i].Hash = 0;
                continue;
            }

            _sceneObjectInfos[i].Hash = so->GetTransformHash();

            if (_sceneObjectInfos[i].BoneIdx == -1)
                continue;

            _sceneObjectTransforms[boneIdx] = sceneObjects[i].So->GetWorldMatrix() * invRootTransform;
            boneIdx++;
        }
    }

    void AnimationProxy::UpdateTime(const Vector<AnimationClipInfo>& clipInfos)
    {
        for (auto& clipInfo : clipInfos)
        {
            AnimationState& state = _layers[clipInfo.LayerIdx].States[clipInfo.StateIdx];

            // Wrap time if looping
            if (state.Loop && state.Length > 0.0f)
                state.Time = Math::Repeat(clipInfo.State.Time, state.Length);
            else
                state.Time = clipInfo.State.Time;

            state.Disabled = (clipInfo.PlaybackType == AnimPlaybackType::None);
        }
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
        if (info.Clips.size() == 0)
            return;

        // Find valid range
        float startPos = 0.0f;
        float endPos = 0.0f;

        for (UINT32 i = 0; i < (UINT32)info.Clips.size(); i++)
        {
            startPos = std::min(startPos, info.Clips[i].Position);
            endPos = std::min(endPos, info.Clips[i].Position);
        }

        float length = endPos - startPos;
        if (Math::ApproxEquals(length, 0.0f) || info.Clips.size() < 2)
        {
            Play(info.Clips[0].Clip);
            return;
        }

        // Clamp or loop time
        bool loop = _defaultWrapMode == AnimWrapMode::Loop;
        if (t < startPos)
        {
            if (loop)
                t = t - std::floor(t / length) * length;
            else // Clamping
                t = startPos;
        }

        if (t > endPos)
        {
            if (loop)
                t = t - std::floor(t / length) * length;
            else // Clamping
                t = endPos;
        }

        // Find keys to blend between
        UINT32 leftKey = 0;
        UINT32 rightKey = 0;

        INT32 start = 0;
        INT32 searchLength = (INT32)info.Clips.size();

        while (searchLength > 0)
        {
            INT32 half = searchLength >> 1;
            INT32 mid = start + half;

            if (t < info.Clips[mid].Position)
            {
                searchLength = half;
            }
            else
            {
                start = mid + 1;
                searchLength -= (half + 1);
            }
        }

        leftKey = std::max(0, start - 1);
        rightKey = std::min(start, (INT32)info.Clips.size() - 1);

        float interpLength = info.Clips[rightKey].Position - info.Clips[leftKey].Position;
        t = (t - info.Clips[leftKey].Position) / interpLength;

        // Add clips and set weights
        for (UINT32 i = 0; i < (UINT32)info.Clips.size(); i++)
        {
            AnimationClipInfo* clipInfo = AddClip(info.Clips[i].Clip, (UINT32)-1, i == 0);
            if (clipInfo != nullptr)
            {
                clipInfo->State.Time = 0.0f;
                clipInfo->State.Stopped = true;
                clipInfo->State.Speed = 0.0f;
                clipInfo->State.WrapMode = AnimWrapMode::Clamp;

                if (i == leftKey)
                    clipInfo->State.Weight = 1.0f - t;
                else if (i == rightKey)
                    clipInfo->State.Weight = t;
                else
                    clipInfo->State.Weight = 0.0f;

                clipInfo->PlaybackType = AnimPlaybackType::Normal;
            }
        }

        _sampleStep = AnimSampleStep::None;
        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
    }

    void Animation::Blend2D(const Blend2DInfo& info, const Vector2& t)
    {
        AnimationClipInfo* topLeftClipInfo = AddClip(info.TopLeftClip, (UINT32)-1, true);
        if (topLeftClipInfo != nullptr)
        {
            topLeftClipInfo->State.Time = 0.0f;
            topLeftClipInfo->State.Stopped = true;
            topLeftClipInfo->State.Speed = 0.0f;
            topLeftClipInfo->State.Weight = (1.0f - t.x) * (1.0f - t.y);
            topLeftClipInfo->State.WrapMode = AnimWrapMode::Clamp;

            topLeftClipInfo->PlaybackType = AnimPlaybackType::Normal;
        }

        AnimationClipInfo* topRightClipInfo = AddClip(info.TopRightClip, (UINT32)-1, false);
        if (topRightClipInfo != nullptr)
        {
            topRightClipInfo->State.Time = 0.0f;
            topRightClipInfo->State.Stopped = true;
            topRightClipInfo->State.Speed = 0.0f;
            topRightClipInfo->State.Weight = t.x * (1.0f - t.y);
            topRightClipInfo->State.WrapMode = AnimWrapMode::Clamp;

            topRightClipInfo->PlaybackType = AnimPlaybackType::Normal;
        }

        AnimationClipInfo* botLeftClipInfo = AddClip(info.BotLeftClip, (UINT32)-1, false);
        if (botLeftClipInfo != nullptr)
        {
            botLeftClipInfo->State.Time = 0.0f;
            botLeftClipInfo->State.Stopped = true;
            botLeftClipInfo->State.Speed = 0.0f;
            botLeftClipInfo->State.Weight = (1.0f - t.x) * t.y;
            botLeftClipInfo->State.WrapMode = AnimWrapMode::Clamp;

            botLeftClipInfo->PlaybackType = AnimPlaybackType::Normal;
        }

        AnimationClipInfo* botRightClipInfo = AddClip(info.BotRightClip, (UINT32)-1, false);
        if (botRightClipInfo != nullptr)
        {
            botRightClipInfo->State.Time = 0.0f;
            botRightClipInfo->State.Stopped = true;
            botRightClipInfo->State.Speed = 0.0f;
            botRightClipInfo->State.Weight = t.x * t.y;
            botRightClipInfo->State.WrapMode = AnimWrapMode::Clamp;

            botRightClipInfo->PlaybackType = AnimPlaybackType::Normal;
        }

        _sampleStep = AnimSampleStep::None;
        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
    }

    void Animation::CrossFade(const HAnimationClip& clip, float fadeLength)
    {
        bool isFading = fadeLength > 0.0f;
        if (!isFading)
        {
            Play(clip);
            return;
        }

        AnimationClipInfo* clipInfo = AddClip(clip, (UINT32)-1, false);
        if (clipInfo != nullptr)
        {
            clipInfo->State.Time = 0.0f;
            clipInfo->State.Speed = _defaultSpeed;
            clipInfo->State.Weight = 1.0f;
            clipInfo->State.WrapMode = _defaultWrapMode;
            clipInfo->PlaybackType = AnimPlaybackType::Normal;

            // Set up fade lengths
            clipInfo->FadeDirection = 1.0f;
            clipInfo->FadeTime = 0.0f;
            clipInfo->FadeLength = fadeLength;

            for (auto& entry : _clipInfos)
            {
                if (entry.State.Layer == (UINT32)-1 && entry.Clip != clip)
                {
                    // If other clips are already cross-fading, we need to persist their current weight before starting
                    // a new crossfade. We do that by adjusting the fade times.
                    if (clipInfo->FadeDirection != 0 && clipInfo->FadeTime < clipInfo->FadeLength)
                    {
                        float t = clipInfo->FadeTime / clipInfo->FadeLength;
                        if (clipInfo->FadeDirection < 0.0f)
                            t = (1.0f - t);

                        clipInfo->State.Weight *= t;
                    }

                    clipInfo->FadeDirection = -1.0f;
                    clipInfo->FadeTime = 0.0f;
                    clipInfo->FadeLength = fadeLength;
                }
            }
        }

        _sampleStep = AnimSampleStep::None;
        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
    }

    void Animation::Sample(const HAnimationClip& clip, float time)
    {
        AnimationClipInfo* clipInfo = AddClip(clip, (UINT32)-1);
        if (clipInfo != nullptr)
        {
            clipInfo->State.Time = time;
            clipInfo->State.Speed = 0.0f;
            clipInfo->State.Weight = 1.0f;
            clipInfo->State.WrapMode = _defaultWrapMode;
            clipInfo->PlaybackType = AnimPlaybackType::Sampled;
        }

        _sampleStep = AnimSampleStep::Frame;
        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
    }

    void Animation::Stop(UINT32 layer)
    {
        Vector<AnimationClipInfo> newClips;
        for (auto& clipInfo : _clipInfos)
        {
            if (clipInfo.State.Layer != layer)
                newClips.push_back(clipInfo);
            else
                _dirty |= (UINT32)AnimDirtyStateFlag::Layout;
        }

        _clipInfos.resize(newClips.size());
        for (UINT32 i = 0; i < (UINT32)newClips.size(); i++)
            _clipInfos[i] = newClips[i];
    }

    void Animation::StopAll()
    {
        _clipInfos.clear();

        _sampleStep = AnimSampleStep::None;
        _dirty |= (UINT32)AnimDirtyStateFlag::Layout;
    }

    AnimationClipInfo* Animation::AddClip(const HAnimationClip& clip, UINT32 layer, bool stopExisting)
    {
        AnimationClipInfo* output = nullptr;
        bool hasExisting = false;

        // Search for existing
        for (auto& clipInfo : _clipInfos)
        {
            if (clipInfo.State.Layer == layer)
            {
                if (clipInfo.Clip == clip)
                    output = &clipInfo;
                else if (stopExisting)
                    hasExisting = true;
            }
        }

        // Doesn't exist or found extra animations, rebuild
        if (output == nullptr || hasExisting)
        {
            Vector<AnimationClipInfo> newClips;
            for (auto& clipInfo : _clipInfos)
            {
                if (!stopExisting || clipInfo.State.Layer != layer || clipInfo.Clip == clip)
                    newClips.push_back(clipInfo);
            }

            if (output == nullptr && clip.GetHandleData() != nullptr)
                newClips.push_back(AnimationClipInfo());

            _clipInfos.resize(newClips.size());
            for (UINT32 i = 0; i < (UINT32)newClips.size(); i++)
                _clipInfos[i] = newClips[i];

            _dirty |= (UINT32)AnimDirtyStateFlag::Layout;
        }

        // If new clip was added, get its address
        if (output == nullptr && clip.GetHandleData() != nullptr)
        {
            AnimationClipInfo& newInfo = _clipInfos.back();
            newInfo.Clip = clip;
            newInfo.State.Layer = layer;

            output = &newInfo;
        }

        return output;
    }

    bool Animation::GetAnimatesRoot() const
    {
        if (_skeleton == nullptr)
            return false;

        UINT32 rootBoneIdx = _skeleton->GetRootBoneIndex();
        if (rootBoneIdx == (UINT32)-1)
            return false;

        String rootBoneName = _skeleton->GetBoneInfo(rootBoneIdx).Name;
        for (auto& entry : _clipInfos)
        {
            if (entry.Clip.GetHandleData())
            {
                HAnimationClip clip = entry.Clip;
                if (!clip->HasRootMotion())
                {
                    AnimationCurveMapping mapping;
                    clip->GetCurveMapping(rootBoneName, mapping);

                    if (mapping.Position != (UINT32)-1)
                        return true;

                    if (mapping.Rotation != (UINT32)-1)
                        return true;

                    if (mapping.Scale != (UINT32)-1)
                        return true;
                }
            }
        }

        return false;
    }

    bool Animation::IsPlaying() const
    {
        for (auto& clipInfo : _clipInfos)
        {
            if (clipInfo.Clip.GetHandleData())
                return true;
        }

        return false;
    }

    bool Animation::GetState(const HAnimationClip& clip, AnimationClipState& state)
    {
        if (!clip.GetHandleData())
            return false;

        for (auto& clipInfo : _clipInfos)
        {
            if (clipInfo.Clip == clip)
            {
                state = clipInfo.State;

                if (state.Layer == (UINT32)-1)
                    state.Layer = 0;
                else
                    state.Layer += 1;

                // Internally we store unclamped time, so clamp/loop it
                float clipLength = 0.0f;
                if (clip.GetHandleData())
                    clipLength = clip->GetLength();

                bool loop = clipInfo.State.WrapMode == AnimWrapMode::Loop;
                AnimationUtility::WrapTime(clipInfo.State.Time, 0.0f, clipLength, loop);

                return true;
            }
        }

        return false;
    }

    void Animation::SetState(const HAnimationClip& clip, AnimationClipState state)
    {
        if (state.Layer == 0)
            state.Layer = (UINT32)-1;
        else
            state.Layer -= 1;

        AnimationClipInfo* clipInfo = AddClip(clip, state.Layer, false);

        if (clipInfo == nullptr)
            return;

        clipInfo->State = state;
        clipInfo->PlaybackType = AnimPlaybackType::Normal;

        _sampleStep = AnimSampleStep::None;
        _dirty |= (UINT32)AnimDirtyStateFlag::Value;
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
        for (auto& clipInfo : _clipInfos)
        {
            if (!clipInfo.Clip.GetInternalPtr())
                continue;

            const Vector<AnimationEvent>& events = clipInfo.Clip->GetEvents();
            bool loop = clipInfo.State.WrapMode == AnimWrapMode::Loop;

            float start = std::max(clipInfo.State.Time - delta, 0.0f);
            float end = clipInfo.State.Time;
            float clipLength = clipInfo.Clip->GetLength();

            float wrappedStart = start;
            float wrappedEnd = end;
            AnimationUtility::WrapTime(wrappedStart, 0.0f, clipLength, loop);
            AnimationUtility::WrapTime(wrappedEnd, 0.0f, clipLength, loop);

            if (!loop)
            {
                for (auto& event : events)
                {
                    if (event.Time >= wrappedStart && (event.Time < wrappedEnd ||
                        (event.Time == clipLength && start < clipLength && end >= clipLength)))
                        OnEventTriggered(clipInfo.Clip, event.Name);
                }
            }
            else
            {
                if (wrappedStart < wrappedEnd)
                {
                    for (auto& event : events)
                    {
                        if (event.Time >= wrappedStart && event.Time < wrappedEnd)
                            OnEventTriggered(clipInfo.Clip, event.Name);
                    }
                }
                else if (wrappedEnd < wrappedStart) // End is looped, but start is not
                {
                    for (auto& event : events)
                    {
                        if ((event.Time >= wrappedStart && event.Time <= clipLength) || (event.Time >= 0 && event.Time < wrappedEnd))
                            OnEventTriggered(clipInfo.Clip, event.Name);
                    }
                }
            }
        }
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
        // Check if any of the clip curves are dirty and advance time, perform fading
        for (auto& clipInfo : _clipInfos)
        {
            float scaledTimeDelta = timeDelta * clipInfo.State.Speed;
            clipInfo.State.Time += scaledTimeDelta;

            HAnimationClip clip = clipInfo.Clip;
            if (clip.GetHandleData())
            {
                // TODO animation : curve version
                _dirty |= (UINT32)AnimDirtyStateFlag::Layout;
            }

            float fadeTime = clipInfo.FadeTime + scaledTimeDelta;
            clipInfo.FadeTime = Math::Clamp(fadeTime, 0.0f, clipInfo.FadeLength);
        }

        if (_sampleStep == AnimSampleStep::None)
        {
            _animProxy->_sampleStep = AnimSampleStep::None;
        }
        else if (_sampleStep == AnimSampleStep::Frame)
        {
            if (_animProxy->_sampleStep == AnimSampleStep::None)
                _animProxy->_sampleStep = AnimSampleStep::Frame;
            else
                _animProxy->_sampleStep = AnimSampleStep::Done;
        }

        if (_dirty |= (UINT32)AnimDirtyStateFlag::Culling)
        {
            _animProxy->_cullEnabled = _cull;
            _dirty &= ~(UINT32)AnimDirtyStateFlag::Culling;
        }

        auto getAnimatedSOList = [&]()
        {
            Vector<AnimatedSceneObject> animatedSO(_sceneObjects.size());
            UINT32 idx = 0;
            for (auto& entry : _sceneObjects)
                animatedSO[idx++] = entry.second;

            return animatedSO;
        };

        bool didFullRebuild = false;
        if ((UINT32)_dirty == 0) // Clean
        {
            _animProxy->UpdateTime(_clipInfos);
        }
        else
        {
            if (_dirty |= (UINT32)AnimDirtyStateFlag::All)
            {
                Vector<AnimatedSceneObject> animatedSOs = getAnimatedSOList();

                _animProxy->Rebuild(_skeleton, _skeletonMask, _clipInfos, animatedSOs);
                didFullRebuild = true;
            }
            else if (_dirty |= (UINT32)AnimDirtyStateFlag::Layout)
            {
                Vector<AnimatedSceneObject> animatedSOs = getAnimatedSOList();

                _animProxy->Rebuild(_clipInfos, animatedSOs);
                didFullRebuild = true;
            }
            else if (_dirty |= (UINT32)AnimDirtyStateFlag::Value)
            {
                _animProxy->UpdateClipInfos(_clipInfos);
            }
        }

        // Check if there are dirty transforms
        if (!didFullRebuild)
        {
            for (UINT32 i = 0; i < _animProxy->_numSceneObjects; i++)
            {
                AnimatedSceneObjectInfo& soInfo = _animProxy->_sceneObjectInfos[i];

                auto iterFind = _sceneObjects.find(soInfo.Id);
                if (iterFind == _sceneObjects.end())
                {
                    assert(false); // Should never happen
                    continue;
                }

                UINT32 hash;

                HSceneObject so = iterFind->second.So;
                if (so.IsDestroyed(true))
                    hash = 0;
                else
                    hash = so->GetTransformHash();

                if (hash != _animProxy->_sceneObjectInfos[i].Hash)
                {
                    Vector<AnimatedSceneObject> animatedSOs = getAnimatedSOList();
                    _animProxy->UpdateTransforms(animatedSOs);
                    break;
                }
            }
        }

        _dirty = (AnimDirtyState)0;
    }

    void Animation::UpdateFromProxy()
    {
        // When sampling a single frame we don't want to keep updating the scene objects so they can be moved through other
        // means (e.g. for the purposes of recording new keyframes if running from the editor).
        const bool disableSOUpdates = _animProxy->_sampleStep == AnimSampleStep::Done;
        if (disableSOUpdates)
            return;

        // If the object was culled, then we have no valid data to read back
        if (_animProxy->_wasCulled)
            return;

        HSceneObject rootSO;

        // Write TRS animation results to relevant SceneObjects
        for (UINT32 i = 0; i < _animProxy->_numSceneObjects; i++)
        {
            AnimatedSceneObjectInfo& soInfo = _animProxy->_sceneObjectInfos[i];

            auto iterFind = _sceneObjects.find(soInfo.Id);
            if (iterFind == _sceneObjects.end())
                continue;

            HSceneObject so = iterFind->second.So;
            if (iterFind->second.CurveName.empty())
                rootSO = so;

            if (so.IsDestroyed(true))
                continue;

            if (soInfo.BoneIdx != -1)
            {
                if (_animProxy->_skeletonPose.HasOverride[soInfo.BoneIdx])
                    continue;

                Vector3 position = _animProxy->_skeletonPose.Positions[soInfo.BoneIdx];
                Quaternion rotation = _animProxy->_skeletonPose.Rotations[soInfo.BoneIdx];
                Vector3 scale = _animProxy->_skeletonPose.Scales[soInfo.BoneIdx];

                const SPtr<Skeleton>& skeleton = _animProxy->_skeleton;

                UINT32 parentBoneIdx = skeleton->GetBoneInfo(soInfo.BoneIdx).Parent;
                if (parentBoneIdx == (UINT32)-1)
                {
                    so->SetPosition(position);
                    so->SetRotation(rotation);
                    so->SetScale(scale);
                }
                else
                {
                    while (parentBoneIdx != (UINT32)-1)
                    {
                        // Update rotation
                        const Quaternion& parentOrientation = _animProxy->_skeletonPose.Rotations[parentBoneIdx];
                        rotation = parentOrientation * rotation;

                        // Update scale
                        const Vector3& parentScale = _animProxy->_skeletonPose.Scales[parentBoneIdx];
                        scale = parentScale * scale;

                        // Update position
                        position = parentOrientation.Rotate(parentScale * position);
                        position += _animProxy->_skeletonPose.Positions[parentBoneIdx];

                        parentBoneIdx = skeleton->GetBoneInfo(parentBoneIdx).Parent;
                    }

                    // Search for root if not already found
                    if (rootSO == nullptr)
                    {
                        for (auto& entry : _sceneObjects)
                        {
                            if (entry.second.CurveName.empty())
                                rootSO = entry.second.So;
                        }
                    }

                    while (rootSO && rootSO.IsDestroyed(true))
                        rootSO = rootSO->GetParent();

                    Vector3 parentPos = Vector3::ZERO;
                    Quaternion parentRot = Quaternion::IDENTITY;
                    Vector3 parentScale = Vector3::ONE;

                    if (!rootSO.IsDestroyed(true))
                    {
                        const Transform& tfrm = rootSO->GetTransform();
                        parentPos = tfrm.GetPosition();
                        parentRot = tfrm.GetRotation();
                        parentScale = tfrm.GetScale();
                    }

                    // Transform from space relative to root's parent to world space
                    rotation = parentRot * rotation;

                    scale = parentScale * scale;

                    position = parentRot.Rotate(parentScale * position);
                    position += parentPos;

                    so->SetWorldPosition(position);
                    so->SetWorldRotation(rotation);
                    so->SetWorldScale(scale);
                }
            }
            else
            {
                if (!_animProxy->_sceneObjectPose.HasOverride[i * 3 + 0])
                    so->SetPosition(_animProxy->_sceneObjectPose.Positions[i]);

                if (!_animProxy->_sceneObjectPose.HasOverride[i * 3 + 1])
                    so->SetRotation(_animProxy->_sceneObjectPose.Rotations[i]);

                if (!_animProxy->_sceneObjectPose.HasOverride[i * 3 + 2])
                    so->SetScale(_animProxy->_sceneObjectPose.Scales[i]);
            }
        }

        // Must ensure that clip in the proxy and current primary clip are the same
        _genericCurveValuesValid = false;
        if (_animProxy->_numLayers > 0 && _animProxy->_layers[0].NumStates > 0)
        {
            const AnimationState& state = _animProxy->_layers[0].States[0];

            if (!state.Disabled && _clipInfos.size() > 0)
            {
                const AnimationClipInfo& clipInfo = _clipInfos[0];

                if (clipInfo.StateIdx == 0 && clipInfo.LayerIdx == 0)
                {
                    // TODO animation : curve version
                    if (clipInfo.Clip.GetHandleData()) 
                    {
                        UINT32 numGenericCurves = (UINT32)clipInfo.Clip->GetCurves()->Generic.size();
                        _genericCurveValuesValid = numGenericCurves == _animProxy->_numGenericCurves;
                    }
                }
            }
        }

        if (_genericCurveValuesValid)
        {
            _genericCurveOutputs.resize(_animProxy->_numGenericCurves);

            memcpy(_genericCurveOutputs.data(), _animProxy->_genericCurveOutputs, _animProxy->_numGenericCurves * sizeof(float));
        }
    }
}
