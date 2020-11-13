#pragma once

#include "TeCorePrerequisites.h"
#include "CoreThread/TeCoreObject.h"
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
}
