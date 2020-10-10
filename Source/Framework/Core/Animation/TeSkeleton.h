#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeTransform.h"
#include "Math/TeQuaternion.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector3.h"

namespace te
{
    /** Information about a single bone used for constructing a skeleton. */
    struct BONE_DESC
    {
        String Lame; /**< Unique name of the bone. */
        UINT32 Parent; /**< Index of the parent bone, if any. -1 if root bone. */

        Transform LocalTrasnform; /**< Local transform of the bone, relative to other bones in the hierarchy. */
        Matrix4 InvBindPose; /**< Inverse bind pose which transforms vertices from their bind pose into local space. */
    };

    /** Contains internal information about a single bone in a Skeleton. */
    struct SkeletonBoneInfo
    {
        String Name; /**< Unique name of the bone. */
        UINT32 Parent; /**< Index of the bone parent, or -1 if root (no parent). */
    };

    /**
     * Contains information about bones required for skeletal animation. Allows caller to evaluate a set of animation
     * clips at a specific time and output the relevant skeleton pose.
     */
    class TE_CORE_EXPORT Skeleton
    {
    public:
        ~Skeleton();

    private:
        Skeleton() = default;
        Skeleton(BONE_DESC* bones, UINT32 numBones);

        UINT32 _numBones = 0;
        Transform* _boneTransforms = nullptr;
        Matrix4* _invBindPoses = nullptr;
        SkeletonBoneInfo* _boneInfo = nullptr;
    };
}
