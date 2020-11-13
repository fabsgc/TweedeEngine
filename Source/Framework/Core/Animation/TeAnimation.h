#pragma once

#include "TeCorePrerequisites.h"
#include "Animation/TeAnimationManager.h"
#include "Serialization/TeSerializable.h"
#include "CoreUtility/TeCoreObject.h"
#include "Animation/TeSkeleton.h"
#include "Animation/TeSkeletonMask.h"
#include "Math/TeVector2.h"
#include "Math/TeAABox.h"

namespace te
{
    /** Determines how an animation clip behaves when it reaches the end. */
    enum class AnimWrapMode
    {
        Loop, /**< Loop around to the beginning/end when the last/first frame is reached. */
        Clamp /**< Clamp to end/beginning, keeping the last/first frame active. */
    };

    /** Contains information about a currently playing animation clip. */
    struct AnimationClipState
    {
        AnimationClipState() = default;

        /** Layer the clip is playing on. Multiple clips can be played simulatenously on different layers. */
        UINT32 Layer = 0;
        float Time = 0.0f; /**< Current time the animation is playing from. */
        float Speed = 1.0f; /**< Speed at which the animation is playing. */
        float Weight = 1.0f; /**< Determines how much of an influence does the clip have on the final pose. */
        /** Determines what happens to other animation clips when a new clip starts playing. */
        AnimWrapMode WrapMode = AnimWrapMode::Loop;
        /**
         * Determines should the time be advanced automatically. Certain type of animation clips don't involve playback
         * (e.g. for blending where animation weight controls the animation).
         */
        bool Stopped = false;
    };

    /** Flags that determine which portion of Animation was changed and needs to be updated. */
    enum class AnimDirtyStateFlag
    {
        Clean = 0,
        Value = 1 << 0,
        Layout = 1 << 1,
        All = 1 << 2,
        Culling = 1 << 3,
        MorphWeights = 1 << 4
    };

    typedef UINT32 AnimDirtyState;

    /** Type of playback for animation clips. */
    enum class AnimPlaybackType
    {
        Normal, /** Play back the animation normally by advancing time. */
        Sampled, /** Sample only a single frame from the animation. */
        None /** Do not play the animation. */
    };

    /** Steps used for progressing through the animation when it is being sampled a single frame. */
    enum class AnimSampleStep
    {
        None, /** No sample. Either no playback at all or normal playback. */
        Frame, /** Sample is being done this frame. */
        Done /** Sample has been performed some previous frame. */
    };


    // TODO


    /**
     * Handles animation playback. Takes one or multiple animation clips as input and evaluates them every animation update
     * tick depending on set properties. The evaluated data is used by the core thread for skeletal animation, by the sim
     * thread for updating attached scene objects and bones (if skeleton is attached), or the data is made available for
     * manual queries in the case of generic animation.
     */
    class TE_CORE_EXPORT Animation : public CoreObject, public Serializable
    {
    public:
        ~Animation();

        /**
         * Changes the skeleton which will the translation/rotation/scale animation values manipulate. If no skeleton is set
         * the animation will only evaluate the generic curves, and the root translation/rotation/scale curves.
         */
        void SetSkeleton(const SPtr<Skeleton>& skeleton);

        /**
         * Sets a mask that allows certain bones from the skeleton to be disabled. Caller must ensure that the mask matches
         * the skeleton assigned to the animation.
         */
        void SetMask(const SkeletonMask& mask);

        /** Returns the unique ID for this animation object. */
		UINT64 GetAnimId() const { return _animId; }

    private:
        friend class AnimationManager;

        Animation();

    private:
        SPtr<Skeleton> _skeleton;
        SkeletonMask _skeletonMask;

        UINT64 _animId;
        AnimDirtyState _dirty = (UINT32)AnimDirtyStateFlag::All;
    };
}
