#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Flags that describe an animation curve. */
    enum class AnimationCurveFlag
    {
        /**
         * If enabled, the curve was imported from an external file and not created within the engine. This will affect
         * how are animation results applied to scene objects (with imported animations it is assumed the curve is
         * animating bones and with in-engine curves it is assumed the curve is animating scene objects).
         */
        ImportedCurve = 1 << 0
    };

    typedef UINT32 AnimationCurveFlags;

    template <class T>
    struct TKeyframe
    {
        T Value; /**< Value of the key. */
        float TimeInSpline; /**< Position of the key along the animation spline. */

        bool operator== (const TKeyframe<T>& rhs) const
        {
            return (Value == rhs.Value && TimeInSpline == rhs.TimeInSpline);
        }

        bool operator!= (const TKeyframe<T>& rhs) const
        {
            return !operator==(rhs);
        }
    };

    template struct TKeyframe<Vector3>;
    template struct TKeyframe<Vector2>;
    template struct TKeyframe<Quaternion>;
    template struct TKeyframe<float>;

    /**
     * Animation spline represented by a set of keyframes, each representing an endpoint of a linear curve. The
     * spline can be evaluated at any time.
     */
    template <class T>
    class TE_CORE_EXPORT TAnimationCurve
    {
    public:
        typedef TKeyframe<T> KeyFrame;

        TAnimationCurve() = default;

        /**
         * Creates a new animation curve.
         * @param[in]	keyframes	Keyframes to initialize the curve with. They must be sorted by time.
         */
        TAnimationCurve(const Vector<KeyFrame>& keyframes);

        /**
         * Evaluate the animation curve at the specified time.
         *
         * @param[in]	time	%Time to evaluate the curve at.
         * @param[in]	loop	If true the curve will loop when it goes past the end or beggining. Otherwise the curve
         *						value will be clamped.
         * @return				Interpolated value from the curve at provided time.
         */
        T Evaluate(float time, bool loop = true) const;

        /**
         * Evaluate the animation curve at the specified time and returns a new keyframe containing the evaluated value
         *
         * @param[in]	time	%Time to evaluate the curve at.
         * @param[in]	loop	If true the curve will loop when it goes past the end or beginning. Otherwise the curve
         *						value will be clamped.
         * @return				Keyframe containing the interpolated value at provided time.
         */
        KeyFrame EvaluateKey(float time, bool loop = true) const;

        /** Returns the time of the first and last keyframe in the curve. */
        std::pair<float, float> GetTimeRange() const;

        /** Calculates the minimal and maximal value of the curve. */
        std::pair<T, T> CalculateRange() const;

        /** Returns the length of the animation curve, from time zero to last keyframe. */
        float GetLength() const { return _end; }

        /** Returns the total number of key-frames in the curve. */
        UINT32 GetNumKeyFrames() const { return (UINT32)_keyframes.size(); }

        /** Returns a keyframe at the specified index. */
        const TKeyframe<T>& GetKeyFrame(UINT32 idx) const { return _keyframes[idx]; }

        /** Returns a list of all keyframes in the curve. */
        const Vector<TKeyframe<T>>& GetKeyFrames() const { return _keyframes; }

        bool operator== (const TAnimationCurve<T>& rhs) const;
        bool operator!= (const TAnimationCurve<T>& rhs) const { return !operator==(rhs); }

    private:
        /**
         * Returns a pair of keys that can be used for interpolating to field the value at the provided time.
         *
         * @param[in]	time			Time for which to find the relevant keys from. It is expected to be clamped to a
         *								valid range within the curve.
         * @param[out]	leftKey			Index of the key to interpolate from.
         * @param[out]	rightKey		Index of the key to interpolate to.
         */
        void FindKeys(float time, UINT32& leftKey, UINT32& rightKey) const;

        /** Returns a keyframe index nearest to the provided time. */
        UINT32 FindKey(float time);

        /**
         * Calculates a key in-between the provided two keys.
         *
         * @param[in]	lhs		Key to interpolate from.
         * @param[in]	rhs		Key to interpolate to.
         * @param[in]	time	Curve time to interpolate the keys at.
         * @return				Interpolated key value.
         */
        KeyFrame EvaluateKey(const KeyFrame& lhs, const KeyFrame& rhs, float time) const;

    private:
        Vector<KeyFrame> _keyframes;
        float _start = 0.0f;
        float _end = 0.0f;
        float _length = 0.0f;
    };

    /** An animation curve and its name. */
    template <class T>
    struct TNamedAnimationCurve
    {
        TNamedAnimationCurve() = default;

        /**
         * Constructs a new named animation curve.
         *
         * @param[in]	name	Name of the curve.
         * @param[in]	curve	Curve containing the animation data.
         */
        TNamedAnimationCurve(const String& name, const TAnimationCurve<T> curve)
            : Name(name)
            , Curve(curve)
        { }

        /**
         * Constructs a new named animation curve.
         *
         * @param[in]	name	Name of the curve.
         * @param[in]	flags	Flags that describe the animation curve.
         * @param[in]	curve	Curve containing the animation data.
         */
        TNamedAnimationCurve(const String& name, AnimationCurveFlags flags, const TAnimationCurve<T> curve)
            : Name(name)
            , Curve(curve)
        { }

        /** Name of the curve. */
        String Name;

        /** Flags that describe the animation curve. */
        AnimationCurveFlags Flags;

        /** Actual curve containing animation data. */
        TAnimationCurve<T> Curve;
    };
}
