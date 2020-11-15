#pragma once

#include "TeCorePrerequisites.h"
#include "Animation/TeAnimationCurve.h"

namespace te
{
    /** Helper class for dealing with animations, animation clips and curves. */
    class TE_CORE_EXPORT AnimationUtility
    {
    public:
        /**
         * Wraps or clamps the provided time value between the provided range.
         *
         * @param[in,out]	time	Time value to wrap/clamp.
         * @param[in]		start	Start of the range.
         * @param[in]		end		End of the range.
         * @param[in]		loop	If true the value will be wrapped, otherwise clamped to range.
         */
        static void WrapTime(float& time, float start, float end, bool loop);

        /**
         * Calculates the total range covered by a set of curves.
         *
         * @param[in]	curves		Curves to calculate range for.
         * @param[out]	xMin		Minimum time value present in the curves.
         * @param[out]	xMax		Maximum time value present in the curves.
         * @param[out]	yMin		Minimum curve value present in the curves.
         * @param[out]	yMax		Maximum curve value present in the curves.
         */
        static void CalculateRange(const Vector<TAnimationCurve<float>>& curves,
            float& xMin, float& xMax, float& yMin, float& yMax);

        /** @copydoc CalculateRange(const Vector<TAnimationCurve<float>>&, float&, float&, float&, float&) */
        static void CalculateRange(const Vector<SPtr<TAnimationCurve<float>>>& curves,
            float& xMin, float& xMax, float& yMin, float& yMax);

        /** Adds a time offset to all keyframes in the provided curve. */
        template<class T>
        static TAnimationCurve<T> OffsetCurve(const TAnimationCurve<T>& curve, float offset);
    };
}
