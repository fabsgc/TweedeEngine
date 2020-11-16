#include "Animation/TeAnimationManager.h"
#include "Animation/TeAnimation.h"
#include "Animation/TeAnimationClip.h"
#include "Utility/TeTime.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeCamera.h"
#include "Mesh/TeMeshData.h"
#include "Mesh/TeMeshUtility.h"

namespace te
{
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

    void AnimationManager::SetUpdateRate(UINT32 fps)
    {
        if (fps == 0) fps = 1;
        _updateRate = 1.0f / fps;
    }

    const EvaluatedAnimationData* AnimationManager::Update()
    {
        if (_paused)
            return &_animData;

        _animationTime += gTime().GetFrameDelta();
        if (_animationTime < _nextAnimationUpdateTime)
            return &_animData;

        _nextAnimationUpdateTime = Math::Floor(_animationTime / _updateRate) * _updateRate + _updateRate;

        float timeDelta = _animationTime - _lastAnimationUpdateTime;
        _lastAnimationUpdateTime = _animationTime;
        _lastAnimationDeltaTime  = timeDelta;

        // Update animation proxies from the latest data
        _proxies.clear();
        for (auto& anim : _animations)
        {
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

        return &_animData;
    }

    void AnimationManager::EvaluateAnimation(AnimationProxy* anim, UINT32& curBoneIdx)
    {
        // TODO animation
    }

    AnimationManager& gAnimationManager()
    {
        return AnimationManager::Instance();
    }
}
