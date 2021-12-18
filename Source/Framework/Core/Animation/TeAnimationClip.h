#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
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
        Count // Keep at end
    };

    struct AnimationCurveMapping;

    /** A set of animation curves representing translation/rotation/scale and generic animation. */
    struct TE_CORE_EXPORT AnimationCurves
    {
        AnimationCurves() = default;

        /**
         * Registers a new curve used for animating position.
         *
         * @param[in]	name		Unique name of the curve. This name will be used mapping the curve to the relevant bone
         *							in a skeleton, if any.
         * @param[in]	curve		Curve to add to the clip.
         */
        void AddPositionCurve(const String& name, const TAnimationCurve<Vector3>& curve);

        /**
         * Registers a new curve used for animating rotation.
         *
         * @param[in]	name		Unique name of the curve. This name will be used mapping the curve to the relevant bone
         *							in a skeleton, if any.
         * @param[in]	curve		Curve to add to the clip.
         */
        void AddRotationCurve(const String& name, const TAnimationCurve<Quaternion>& curve);

        /**
         * Registers a new curve used for animating scale.
         *
         * @param[in]	name		Unique name of the curve. This name will be used mapping the curve to the relevant bone
         *							in a skeleton, if any.
         * @param[in]	curve		Curve to add to the clip.
         */
        void AddScaleCurve(const String& name, const TAnimationCurve<Vector3>& curve);

        /**
         * Registers a new curve used for generic animation.
         *
         * @param[in]	name		Unique name of the curve. This can be used for retrieving the value of the curve
         *							from animation.
         * @param[in]	curve		Curve to add to the clip.
         */
        void AddGenericCurve(const String& name, const TAnimationCurve<float>& curve);

        /** Removes an existing curve from the clip. */
        void RemovePositionCurve(const String& name);

        /** Removes an existing curve from the clip. */
        void RemoveRotationCurve(const String& name);

        /** Removes an existing curve from the clip. */
        void RemoveScaleCurve(const String& name);

        /** Removes an existing curve from the clip. */
        void RemoveGenericCurve(const String& name);

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

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_AnimationClip; }

        /** @copydoc SetCurves */
        SPtr<AnimationCurves> GetCurves() const { return _curves; }

        /**
         * A set of all curves stored in the animation. Returned value will not be updated if the animation clip curves are
         * added or removed, as it is a copy of clip's internal values.
         */
        void SetCurves(const AnimationCurves& curves);

        /** @copydoc SetEvents */
        const Vector<AnimationEvent>& GetEvents() const { return _events; }

        /** A set of all events to be triggered as the animation is playing. */
        void SetEvents(const Vector<AnimationEvent>& events) { _events = events; }

        /**
         * Returns a set of curves containing motion of the root bone. This allows the user to evaluate the root bone
         * animation curves manually, instead of through the normal animation process. This property is only available
         * if animation clip was imported with root motion import enabled.
         */
        SPtr<RootMotion> GetRootMotion() const { return _rootMotion; }

        /** Checks if animation clip has root motion curves separate from the normal animation curves. */
        bool HasRootMotion() const;

        /**
         * Maps skeleton bone names to animation curve names, and returns a set of indices that can be easily used for
         * locating an animation curve based on the bone index.
         *
         * @param[in]	skeleton	Skeleton to create the mapping for.
         * @param[out]	mapping		Pre-allocated array that will receive output animation clip indices. The array must
         *							be large enough to store an index for every bone in the @p skeleton. Bones that have
         *							no related animation curves will be assigned value -1.
         */
        void GetBoneMapping(const Skeleton& skeleton, AnimationCurveMapping* mapping) const;

        /**
         * Attempts to find translation/rotation/scale curves with the specified name and fills the mapping structure with
         * their indices, which can then be used for quick lookup.
         *
         * @param[in]	name		Name of the curves to look up.
         * @param[out]	mapping		Triple containing the translation/rotation/scale indices of the found curves. Indices
         *							will be -1 for curves that haven't been found.
         */
        void GetCurveMapping(const String& name, AnimationCurveMapping& mapping) const;

        /** Returns the length of the animation clip, in seconds. */
        float GetLength() const { return _length; }

        /** @copydoc SetSampleRate */
        float GetSampleRate() const { return _sampleRate; }

        /**
         * Checks are the curves contained within the clip additive. Additive clips are intended to be added on top of
         * other clips.
         */
        bool IsAdditive() const { return _isAdditive; }

        /**
         * Number of samples per second the animation clip curves were sampled at. This value is not used by the animation
         * clip or curves directly since unevenly spaced keyframes are supported. But it can be of value when determining
         * the original sample rate of an imported animation or similar.
         */
        void SetSampleRate(float sampleRate) { _sampleRate = sampleRate; }

        /**
         * Creates an AnimationClip with no data. You must populate its data manually followed by a call to initialize().
         *
         * @note	For serialization use only.
         */
        static SPtr<AnimationClip> CreateEmpty();

        /**
         * Creates an animation clip with no curves. After creation make sure to register some animation curves before
         * using it.
         */
        static HAnimationClip Create(bool isAdditive = false);

        /**
         * Creates an animation clip with specified curves.
         *
         * @param[in]	curves		Curves to initialize the animation with.
         * @param[in]	isAdditive	Determines does the clip contain additive curve data. This will change the behaviour
		 *							how is the clip blended with other animations.
         * @param[in]	sampleRate	If animation uses evenly spaced keyframes, number of samples per second. Not relevant
         *							if keyframes are unevenly spaced.
         * @param[in]	rootMotion	Optional set of curves that can be used for animating the root bone. Not used by the
         *							animation system directly but is instead provided to the user for manual evaluation.
         */
        static HAnimationClip Create(const SPtr<AnimationCurves>& curves, bool isAdditive = false, float sampleRate = 1.0f,
            const SPtr<RootMotion>& rootMotion = nullptr);

        /** Creates a new AnimationClip without initializing it. Use Create() for normal use. */
        static SPtr<AnimationClip> CreatePtr(const SPtr<AnimationCurves>& curves, bool isAdditive = false,
            float sampleRate = 1.0f, const SPtr<RootMotion>& rootMotion = nullptr);

    protected:
        AnimationClip();
        AnimationClip(const SPtr<AnimationCurves>& curves, bool isAdditive, float sampleRate,
            const SPtr<RootMotion>& rootMotion);

        /** @copydoc Resource::Initialize */
        void Initialize() override;

        /** Creates a name -> curve index mapping for quicker curve lookup by name. */
        void BuildNameMapping();

        /** Calculate the length of the clip based on assigned curves. */
        void CalculateLength();

    protected:
        /**
         * Contains all the animation curves in the clip. It's important this field is immutable so it may be used on other
         * threads. This means any modifications to the field will require a brand new data structure to be generated and
         * all existing data copied (plus the modification).
         */
        SPtr<AnimationCurves> _curves;

        /**
         * A set of curves containing motion of the root bone. If this is non-empty it should be true that mCurves does not
         * contain animation curves for the root bone. Root motion will not be evaluated through normal animation process
         * but is instead provided for the user for manual evaluation.
         */
        SPtr<RootMotion> _rootMotion;

        /**
         * Contains a map from curve name to curve index. Indices are stored as specified in CurveType enum.
         */
        UnorderedMap<String, std::array<UINT32, (int)CurveType::Count>> _nameMapping;

        Vector<AnimationEvent> _events;
        bool _isAdditive;
        float _length;
        float _sampleRate;
    };
}
