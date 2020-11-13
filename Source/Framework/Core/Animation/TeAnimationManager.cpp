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

    AnimationManager& gAnimation()
    {
        return AnimationManager::Instance();
    }
}
