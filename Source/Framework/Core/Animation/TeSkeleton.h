#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeTransform.h"
#include "Math/TeQuaternion.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector3.h"
#include "Scene/TeTransform.h"
#include "Serialization/TeSerializable.h"
#include "CoreUtility/TeCoreObject.h"
#include "Animation/TeSkeletonMask.h"
#include "Animation/TeAnimationClip.h"

namespace te
{
    /**
     * Contains indices for position/rotation/scale animation curves. Used for quick mapping of bones in a skeleton to
     * relevant animation curves.
     */
    struct AnimationCurveMapping
    {
        UINT32 Position;
        UINT32 Rotation;
        UINT32 Scale;
    };

    /** Information about a single bone used for constructing a skeleton. */
    struct BONE_DESC
    {
        String Name; /**< Unique name of the bone. */
        UINT32 Parent = 0; /**< Index of the parent bone, if any. -1 if root bone. */

        Transform LocalTfrm; /**< Local transform of the bone, relative to other bones in the hierarchy. */
        Matrix4 InvBindPose = Matrix4(); /**< Inverse bind pose which transforms vertices from their bind pose into local space. */
    };

    /** Contains information about a single playing animation clip. */
    struct AnimationState
    {
        SPtr<AnimationCurves> Curves; /**< All curves in the animation clip. */
        float Length; /**< Total length of the animation clip in seconds (same as the length of the longest animation curve). */
        AnimationCurveMapping* BoneToCurveMapping; /**< Mapping of bone indices to curve indices for quick lookup .*/
        AnimationCurveMapping* SoToCurveMapping; /**< Mapping of scene object indices to curve indices for quick lookup. */

        float Time; /**< Time to evaluate the curve at. */
        float Weight; /**< Determines how much of an influence will this clip have in regard to others in the same layer. */
        bool Loop; /**< Determines should the animation loop (wrap) once ending or beginning frames are passed. */
        bool Disabled; /**< If true the clip state will not be evaluated. */
    };

    /** Contains animation states for a single animation layer. */
    struct AnimationStateLayer
    {
        AnimationState* States; /**< Array of animation states in the layer. */
        UINT32 NumStates; /**< Number of states in @p states. */

        UINT8 Index; /**< Unique index of the animation layer. */

        /**
         * If true animations from this layer will be added on top of other layers using the per-state weights. If false
         * the weights will be normalized, animations will be blended with each other according to the normalized weights
         * and then added on top of other layers.
         */
        bool Additive;
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
        LocalSkeletonPose(LocalSkeletonPose&& other)  noexcept;
        ~LocalSkeletonPose();

        LocalSkeletonPose& operator=(const LocalSkeletonPose& other) = delete;
        LocalSkeletonPose& operator=(LocalSkeletonPose&& other) noexcept;

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
    class TE_CORE_EXPORT Skeleton : public CoreObject, public Serializable
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
        Transform ComputeBoneTransform(UINT32 idx) const;

        /**
         * Outputs a skeleton pose containing required transforms for transforming the skeleton to the values specified by
         * the provided animation clip evaluated at the specified time.
         *
         * @param[out]	pose		Output pose containing the requested transforms. Must be pre-allocated with enough space
         *							to hold all the bone matrices of this skeleton.
         * @param[in]	mask		Mask that filters which skeleton bones are enabled or disabled.
         * @param[out]	localPose	Output pose containing the local transforms. Must be pre-allocated with enough space
         *							to hold all the bone data of this skeleton.
         * @param[in]	clip		Clip to evaluate.
         * @param[in]	time		Time to evaluate the clip with.
         * @param[in]	loop		Determines should the time be looped (wrapped) if it goes past the clip start/end.
         *
         * @note	It is more efficient to use the other getPose overload as sequential calls can benefit from animation
         *			evaluator cache.
         */
        void GetPose(Matrix4* pose, LocalSkeletonPose& localPose, const SkeletonMask& mask, const AnimationClip& clip,
            float time, bool loop = true);

        /**
         * Outputs a skeleton pose containing required transforms for transforming the skeleton to the values specified by
         * the provided set of animation curves.
         *
         * @param[out]	pose		Output pose containing the requested transforms. Must be pre-allocated with enough space
         *							to hold all the bone matrices of this skeleton.
         * @param[in]	mask		Mask that filters which skeleton bones are enabled or disabled.
         * @param[out]	localPose	Output pose containing the local transforms. Must be pre-allocated with enough space
         *							to hold all the bone data of this skeleton.
         * @param[in]	layers		One or multiple layers, containing one or multiple animation states to evaluate.
         * @param[in]	numLayers	Number of layers in the @p layers array.
         */
        void GetPose(Matrix4* pose, LocalSkeletonPose& localPose, const SkeletonMask& mask,
            const AnimationStateLayer* layers, UINT32 numLayers);

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
