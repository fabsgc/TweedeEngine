#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /**
     * Contains a bitfield that determines which skeleton bones are enabled or disabled during skeletal animation. Use
     * SkeletonMaskBuilder to create a mask for a specific skeleton.
     */
    class TE_CORE_EXPORT SkeletonMask
    {
    public:
        SkeletonMask() = default;
        SkeletonMask(UINT32 numBones);

        /**
         * Checks is the bone at the specified index enabled. Caller is expected to know which skeleton is the skeleton
         * mask tied with, in order to determine the bone index.
         */
        bool IsEnabled(UINT32 boneIdx) const;

    private:
        friend class SkeletonMaskBuilder;

        Vector<bool> _isDisabled;
    };

    /** Builds a SkeletonMask for a specific skeleton. */
    class TE_CORE_EXPORT SkeletonMaskBuilder
    {
    public:
        SkeletonMaskBuilder(const SPtr<Skeleton>& skeleton);

        /** Enables or disables a bone with the specified name. */
        void SetBoneState(const String& name, bool enabled);

        /** Teturns the built skeleton mask. */
        SkeletonMask GetMask() const { return _mask; }

    private:
        SPtr<Skeleton> _skeleton;
        SkeletonMask _mask;
    };
}
