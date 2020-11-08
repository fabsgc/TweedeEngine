#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeTransform.h"
#include "Math/TeQuaternion.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector3.h"
#include "Scene/TeTransform.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /** Information about a single bone used for constructing a skeleton. */
    struct BONE_DESC
    {
        String Name; /**< Unique name of the bone. */
        UINT32 Parent; /**< Index of the parent bone, if any. -1 if root bone. */

        Transform LocalTfrm; /**< Local transform of the bone, relative to other bones in the hierarchy. */
        Matrix4 InvBindPose; /**< Inverse bind pose which transforms vertices from their bind pose into local space. */
    };

    /**
     * Contains local translation, rotation and scale values for each bone in a skeleton, after being evaluated at a
     * specific time of an animation.  All values are stored in the same order as the bones in the skeleton they were
     * created by.
     */
    struct LocalSkeletonPose
    {
        LocalSkeletonPose() = default;
        LocalSkeletonPose(UINT32 numBones, bool individualOverride = false);
        LocalSkeletonPose(UINT32 numPos, UINT32 numRot, UINT32 numScale);
        LocalSkeletonPose(const LocalSkeletonPose& other) = delete;
        LocalSkeletonPose(LocalSkeletonPose&& other);
        ~LocalSkeletonPose();

        LocalSkeletonPose& operator=(const LocalSkeletonPose& other) = delete;
        LocalSkeletonPose& operator=(LocalSkeletonPose&& other);

        Vector3* Positions = nullptr; /**< Local bone positions at specific animation time. */
        Quaternion* Rotations = nullptr; /**< Local bone rotations at specific animation time. */
        Vector3* Scales = nullptr; /**< Local bone scales at specific animation time. */
        bool* HasOverride = nullptr; /**< True if the bone transform was overriden externally (local pose was ignored). */
        UINT32 NumBones = 0; /**< Number of bones in the pose. */
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
    class TE_CORE_EXPORT Skeleton : public Serializable
    {
    public:
        ~Skeleton();

        /** Returns the total number of bones in the skeleton. */
        UINT32 GetNumBones() const { return _numBones; }

        /** Returns information about a bone at the provided index. */
        const SkeletonBoneInfo& GetBoneInfo(UINT32 idx) const { return _bonesInfo[idx]; }

        /** Searches all bones to find a root bone. Returns -1 if no root can be found. */
        UINT32 GetRootBoneIndex() const;

        /** Returns the inverse bind pose for the bone at the provided index. */
        const Matrix4& GetInvBindPose(UINT32 idx) const { return _invBindPoses[idx]; }

        /** Calculates the bind-pose transform of the bone at the specified index. */
        Transform CalcBoneTransform(UINT32 idx) const;

        /**
         * Creates a new Skeleton.
         *
         * @param[in]	bones		An array of bones to initialize the skeleton with. Data will be copied.
         * @param[in]	numBones	Number of bones in the @p bones array.
         */
        static SPtr<Skeleton> Create(BONE_DESC* bones, UINT32 numBones);

        /**
         * Creates a Skeleton with no data. You must populate its data manually.
         * @note	For serialization use only.
         */
        static SPtr<Skeleton> CreateEmpty();

    private:
        Skeleton();
        Skeleton(BONE_DESC* bones, UINT32 numBones);

        UINT32 _numBones = 0;
        Transform* _boneTransforms = nullptr;
        Matrix4* _invBindPoses = nullptr;
        SkeletonBoneInfo* _bonesInfo = nullptr;
    };
}
