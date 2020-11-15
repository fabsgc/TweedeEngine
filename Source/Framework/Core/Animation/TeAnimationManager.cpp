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

    void AnimationManager::SetUpdateRate(UINT32 fps)
    {
        if (fps == 0) fps = 1;
        _updateRate = 1.0f / fps;
    }

    const EvaluatedAnimationData* AnimationManager::Update()
    {
        const EvaluatedAnimationData* output = nullptr;
        return output;
        // TODO animation
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
