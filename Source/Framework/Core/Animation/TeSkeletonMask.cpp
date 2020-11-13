#include "Animation/TeSkeletonMask.h"
#include "Animation/TeSkeleton.h"

namespace te
{
    SkeletonMask::SkeletonMask(UINT32 numBones)
        : _isDisabled(numBones)
    { }

    bool SkeletonMask::IsEnabled(UINT32 boneIdx) const
    {
        if (boneIdx >= (UINT32)_isDisabled.size())
            return true;

        return !_isDisabled[boneIdx];
    }

    SkeletonMaskBuilder::SkeletonMaskBuilder(const SPtr<Skeleton>& skeleton)
        : _skeleton(skeleton)
        , _mask(skeleton->GetNumBones())
    { }

    void SkeletonMaskBuilder::SetBoneState(const String& name, bool enabled)
    {
        UINT32 numBones = _skeleton->GetNumBones();
        for(UINT32 i = 0; i < numBones; i++)
        {
            if(_skeleton->GetBoneInfo(i).Name == name)
            {
                _mask._isDisabled[i] = !enabled;
                break;
            }
        }
    }
}
