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
        ImportedCurve = 1 << 0,
        /** Signifies the curve is used to animate between different frames within a morph channel. In range [0, 1]. */
        MorphFrame = 1 << 1,
        /** Signifies the curve is used to adjust the weight of a morph channel. In range [0, 1]. */
        MorphWeight = 1 << 2
    };

    typedef UINT32 AnimationCurveFlags;

    template <class T>
    struct TKeyframe
    {
        T Value; /**< Value of the key. */
        T InTangent; /**< Input tangent (going from the previous key to this one) of the key. */
        T OutTangent; /**< Output tangent (going from this key to next one) of the key. */
        float TimeInSpline; /**< Position of the key along the animation spline. */

        bool operator== (const TKeyframe<T>& rhs) const
        {
            return (Value == rhs.Value && InTangent == rhs.InTangent && OutTangent == rhs.OutTangent && TimeInSpline == rhs.TimeInSpline);
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
     * Animation spline represented by a set of keyframes, each representing an endpoint of a cubic hermite curve. The
     * spline can be evaluated at any time, and uses caching to speed up multiple sequential evaluations.
     */
    template <class T>
    class TE_CORE_EXPORT TAnimationCurve // Note: Curves are expected to be immutable for threading purposes
    {
    public:
        typedef TKeyframe<T> KeyFrame;

        TAnimationCurve() = default;

        /**
         * Creates a new animation curve.
         * @param[in]	keyframes	Keyframes to initialize the curve with. They must be sorted by time.
         */
        TAnimationCurve(const Vector<KeyFrame>& keyframes);

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
