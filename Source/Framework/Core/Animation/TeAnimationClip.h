#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"
#include "Animation/TeAnimationCurve.h"
#include <array>

namespace te
{
    /** Types of curves in an AnimationClip. */
    enum class CurveType
    {
        Position,
        Rotation,
        Scale,
        Generic,
        MorphFrame,
        MorphWeight,
        Count // Keep at end
    };

    struct AnimationCurveMapping;

    /** A set of animation curves representing translation/rotation/scale and generic animation. */
    struct TE_CORE_EXPORT AnimationCurves
    {
        AnimationCurves() = default;

        /** Curves for animating scene object's position. */
        Vector<TNamedAnimationCurve<Vector3>> Position;

        /** Curves for animating scene object's rotation. */
        Vector<TNamedAnimationCurve<Quaternion>> Rotation;

        /** Curves for animating scene object's scale. */
        Vector<TNamedAnimationCurve<Vector3>> Scale;

        /** Curves for animating generic component properties. */
        Vector<TNamedAnimationCurve<float>> Generic;
    };

    /** Contains a set of animation curves used for moving and rotating the root bone. */
    struct RootMotion
    {
        RootMotion() = default;
        RootMotion(const TAnimationCurve<Vector3>& position, const TAnimationCurve<Quaternion>& rotation)
            : Position(position)
            , Rotation(rotation)
        { }

        /** Animation curve representing the movement of the root bone. */
        TAnimationCurve<Vector3> Position;

        /** Animation curve representing the rotation of the root bone. */
        TAnimationCurve<Quaternion> Rotation;
    };

    /** Event that is triggered when animation reaches a certain point. */
    struct AnimationEvent
    {
        AnimationEvent() = default;

        /**
         * Constructs a new animation event.
         *
         * @param[in]	name	Name used to identify the event when triggered.
         * @param[in]	time	Time at which to trigger the event, in seconds.
         */
        AnimationEvent(const String& name, float time)
            : Name(name)
            , Time(time)
        { }

        /** Name used to identify the event when triggered. */
        String Name;

        /** Time at which to trigger the event, in seconds. */
        float Time = 0.0f;
    };

    /**
     * Contains animation curves for translation/rotation/scale of scene objects/skeleton bones, as well as curves for
     * generic property animation.
     */
    class TE_CORE_EXPORT AnimationClip : public Resource
    {
    public:
        virtual ~AnimationClip() = default;

    protected:
        AnimationClip();
        AnimationClip(const SPtr<AnimationCurves>& curves, bool isAdditive, UINT32 sampleRate,
            const SPtr<RootMotion>& rootMotion);

    };
}
