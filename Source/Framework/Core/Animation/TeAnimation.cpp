#include "TeAnimation.h"

namespace te
{
    Animation::Animation()
        : Serializable(TID_Animation)
    {
        _animId = AnimationManager::Instance().RegisterAnimation(this);
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
}
