#include "Animation/TeAnimationManager.h"
#include "Animation/TeAnimation.h"
#include "Animation/TeAnimationClip.h"
#include "Utility/TeTime.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeCamera.h"
#include "Mesh/TeMeshData.h"
#include "Mesh/TeMeshUtility.h"
#include "TeCoreApplication.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(AnimationManager)

    AnimationManager::AnimationManager()
    { }

    UINT64 AnimationManager::RegisterAnimation(Animation* anim)
    {
        _animations[_nextId] = anim;
        return _nextId++;
    }

    void AnimationManager::UnregisterAnimation(UINT64 animId)
    {
        _animations.erase(animId);
    }

    void AnimationManager::SetPaused(bool paused)
    {
        _paused = paused;
    }

    void AnimationManager::TogglePaused()
    {
        _paused = !_paused;
    }

    void AnimationManager::SetAnimDataDirty()
    {
        _animDataDirty = true;
    }

    void AnimationManager::SetUpdateRate(UINT32 fps)
    {
        if (fps == 0) fps = 1;
        _updateRate = 1.0f / fps;
    }

    const EvaluatedAnimationData* AnimationManager::Update()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Animation);
        if (IsPaused() || !isRunning)
            return &_animData;

        _animationTime += gTime().GetFrameDelta();
        if (_animationTime < _nextAnimationUpdateTime && !_animDataDirty)
            return &_animData;

        _nextAnimationUpdateTime = Math::Floor(_animationTime / _updateRate) * _updateRate + _updateRate;

        float timeDelta = _animationTime - _lastAnimationUpdateTime;
        _lastAnimationUpdateTime = _animationTime;
        _lastAnimationDeltaTime  = timeDelta;

        // Update animation proxies from the latest data
        _proxies.clear();
        for (auto& anim : _animations)
        {
            if (anim.second->GetNumClips() == 0)
                continue;

            anim.second->UpdateAnimProxy(timeDelta);
            _proxies.push_back(anim.second->_animProxy);
        }

        // Build frustums for culling
        _cullFrustums.clear();

        auto& allCameras = gSceneManager().GetAllCameras();
        for (auto& entry : allCameras)
        {
            // Note: This should also check on-demand cameras as there's no point in updating them if they wont render this frame
            if (entry.second->GetRenderSettings()->OverlayOnly)
                continue;

            if (!entry.second->GetActive())
                continue;

            UINT32 cameraFlags = entry.second->GetFlags();
            if ((cameraFlags & (UINT32)CameraFlag::OnDemand))
            {
                // if camera is on demand, we check if user has asked for redraw
                UINT32 cameraDirtyFlag = entry.second->GetCoreDirtyFlags();
                if (!(cameraDirtyFlag & (INT32)CameraDirtyFlag::Redraw))
                    continue;
            }

            _cullFrustums.push_back(entry.second->GetWorldFrustum());
        }

        // Prepare the write buffer
        UINT32 totalNumBones = 0;
        for (auto& anim : _proxies)
        {
            if (anim->_skeleton != nullptr)
                totalNumBones += anim->_skeleton->GetNumBones();
        }

        // Prepare the write buffer
        _animData.Transforms.resize(totalNumBones);
        _animData.Infos.clear();

        UINT32 curBoneIdx = 0;
        for (auto& anim : _proxies)
        {
            UINT32 boneIdx = curBoneIdx;
            EvaluateAnimation(anim.get(), boneIdx);

            if (anim->_skeleton != nullptr)
                curBoneIdx += anim->_skeleton->GetNumBones();
        }

        // Trigger events and update attachments (for the data we just evaluated)
        for (auto& anim : _animations)
        {
            anim.second->UpdateFromProxy();
            anim.second->TriggerEvents(timeDelta);
        }

        _animDataDirty = false;
        return &_animData;
    }

    void AnimationManager::EvaluateAnimation(AnimationProxy* anim, UINT32& curBoneIdx)
    {
        // Culling
        if (anim->_cullEnabled)
        {
            bool isVisible = false;
            for (auto& frustum : _cullFrustums)
            {
                if (frustum.Intersects(anim->_bounds))
                {
                    isVisible = true;
                    break;
                }
            }

            if (!isVisible)
            {
                anim->_wasCulled = true;
                return;
            }
        }

        anim->_wasCulled = false;

        EvaluatedAnimationData::AnimInfo animInfo;
        bool hasAnimInfo = false;

        // Evaluate skeletal animation
        if (anim->_skeleton != nullptr)
        {
            UINT32 numBones = anim->_skeleton->GetNumBones();

            EvaluatedAnimationData::PoseInfo& poseInfo = animInfo.PoseInfos;
            poseInfo.AnimId = anim->Id;
            poseInfo.StartIdx = curBoneIdx;
            poseInfo.NumBones = numBones;

            memset(anim->_skeletonPose.HasOverride, 0, sizeof(bool) * anim->_skeletonPose.NumBones);
            Matrix4* boneDst = _animData.Transforms.data() + curBoneIdx;

            // Copy transforms from mapped scene objects
            UINT32 boneTfrmIdx = 0;
            for (UINT32 i = 0; i < anim->_numSceneObjects; i++)
            {
                const AnimatedSceneObjectInfo& soInfo = anim->_sceneObjectInfos[i];

                if (soInfo.BoneIdx == -1)
                    continue;

                boneDst[soInfo.BoneIdx] = anim->_sceneObjectTransforms[boneTfrmIdx];
                anim->_skeletonPose.HasOverride[soInfo.BoneIdx] = true;
                boneTfrmIdx++;
            }

            // Animate bones
            anim->_skeleton->GetPose(boneDst, anim->_skeletonPose, anim->_skeletonMask, anim->_layers, anim->_numLayers);

            curBoneIdx += numBones;
            hasAnimInfo = true;
        }
        else
        {
            EvaluatedAnimationData::PoseInfo& poseInfo = animInfo.PoseInfos;
            poseInfo.AnimId = anim->Id;
            poseInfo.StartIdx = 0;
            poseInfo.NumBones = 0;
        }

        // Reset mapped SO transform
        for (UINT32 i = 0; i < anim->_sceneObjectPose.NumBones; i++)
        {
            anim->_sceneObjectPose.Positions[i] = Vector3::ZERO;
            anim->_sceneObjectPose.Rotations[i] = Quaternion::IDENTITY;
            anim->_sceneObjectPose.Scales[i] = Vector3::ONE;
        }

        // Update mapped scene objects
        memset(anim->_sceneObjectPose.HasOverride, 1, sizeof(bool) * 3 * anim->_numSceneObjects);

        // Update scene object transforms
        for (UINT32 i = 0; i < anim->_numSceneObjects; i++)
        {
            const AnimatedSceneObjectInfo& soInfo = anim->_sceneObjectInfos[i];

            // We already evaluated bones
            if (soInfo.BoneIdx != -1)
                continue;

            if (soInfo.LayerIdx == -1 || soInfo.StateIdx == -1)
                continue;

            const AnimationState& state = anim->_layers[soInfo.LayerIdx].States[soInfo.StateIdx];
            if (state.Disabled)
                continue;

            {
                UINT32 curveIdx = soInfo.CurveIndices.Position;
                if (curveIdx != (UINT32)-1)
                {
                    const TAnimationCurve<Vector3>& curve = state.Curves->Position[curveIdx].Curve;
                    anim->_sceneObjectPose.Positions[curveIdx] = curve.Evaluate(state.Time, false);
                    anim->_sceneObjectPose.HasOverride[i * 3 + 0] = false;
                }
            }

            {
                UINT32 curveIdx = soInfo.CurveIndices.Rotation;
                if (curveIdx != (UINT32)-1)
                {
                    const TAnimationCurve<Quaternion>& curve = state.Curves->Rotation[curveIdx].Curve;
                    anim->_sceneObjectPose.Rotations[curveIdx] = curve.Evaluate(state.Time, false);
                    anim->_sceneObjectPose.Rotations[curveIdx].Normalize();
                    anim->_sceneObjectPose.HasOverride[i * 3 + 1] = false;
                }
            }

            {
                UINT32 curveIdx = soInfo.CurveIndices.Scale;
                if (curveIdx != (UINT32)-1)
                {
                    const TAnimationCurve<Vector3>& curve = state.Curves->Scale[curveIdx].Curve;
                    anim->_sceneObjectPose.Scales[curveIdx] = curve.Evaluate(state.Time, false);
                    anim->_sceneObjectPose.HasOverride[i * 3 + 2] = false;
                }
            }
        }

        // Update generic curves
        // Note: No blending for generic animations, just use first animation
        if (anim->_numLayers > 0 && anim->_layers[0].NumStates > 0)
        {
            const AnimationState& state = anim->_layers[0].States[0];
            if (!state.Disabled)
            {
                UINT32 numCurves = (UINT32)state.Curves->Generic.size();
                for (UINT32 i = 0; i < numCurves; i++)
                {
                    const TAnimationCurve<float>& curve = state.Curves->Generic[i].Curve;
                    anim->_genericCurveOutputs[i] = curve.Evaluate(state.Time, false);
                }
            }
        }

        if (hasAnimInfo)
            _animData.Infos[anim->Id] = animInfo;
    }

    AnimationManager& gAnimationManager()
    {
        return AnimationManager::Instance();
    }
}
