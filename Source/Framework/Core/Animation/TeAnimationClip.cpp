#include "Animation/TeAnimationClip.h"
#include "Resources/TeResource.h"
#include "Animation/TeSkeleton.h"

namespace te
{
    AnimationClip::AnimationClip()
        : Resource(TID_AnimationClip)
    { }

    AnimationClip::AnimationClip(const SPtr<AnimationCurves>& curves, bool isAdditive, UINT32 sampleRate,
        const SPtr<RootMotion>& rootMotion)
        : Resource(TID_AnimationClip)
    { }
}
