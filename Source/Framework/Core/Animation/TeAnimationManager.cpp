#include "Animation/TeAnimationManager.h"
#include "Animation/TeAnimation.h"

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

    AnimationManager& gAnimationManager()
    {
        return AnimationManager::Instance();
    }
}
