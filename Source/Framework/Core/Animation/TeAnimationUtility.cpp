#include "Animation/TeAnimationUtility.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    void AnimationUtility::WrapTime(float& time, float start, float end, bool loop)
    {
        float length = end - start;

        if (Math::ApproxEquals(length, 0.0f))
        {
            time = 0.0f;
            return;
        }

        // Clamp to start or loop
        if (time < start)
        {
            if (loop)
                time = time + (std::floor(end - time) / length) * length;
            else // Clamping
                time = start;
        }

        // Clamp to end or loop
        if (time > end)
        {
            if (loop)
                time = time - std::floor((time - start) / length) * length;
            else // Clamping
                time = end;
        }
    }

    void AnimationUtility::CalculateRange(const Vector<TAnimationCurve<float>>& curves, float& xMin, float& xMax,
        float& yMin, float& yMax)
    {
        xMin = std::numeric_limits<float>::infinity();
        xMax = -std::numeric_limits<float>::infinity();
        yMin = std::numeric_limits<float>::infinity();
        yMax = -std::numeric_limits<float>::infinity();

        for (auto& entry : curves)
        {
            const auto timeRange = entry.GetTimeRange();
            const auto valueRange = entry.CalculateRange();

            xMin = std::min(xMin, timeRange.first);
            xMax = std::max(xMax, timeRange.second);
            yMin = std::min(yMin, valueRange.first);
            yMax = std::max(yMax, valueRange.second);
        }

        if (xMin == std::numeric_limits<float>::infinity())
            xMin = 0.0f;

        if (xMax == -std::numeric_limits<float>::infinity())
            xMax = 0.0f;

        if (yMin == std::numeric_limits<float>::infinity())
            yMin = 0.0f;

        if (yMax == -std::numeric_limits<float>::infinity())
            yMax = 0.0f;
    }

    void AnimationUtility::CalculateRange(const Vector<SPtr<TAnimationCurve<float>>>& curves, float& xMin, float& xMax,
        float& yMin, float& yMax)
    {
        xMin = std::numeric_limits<float>::infinity();
        xMax = -std::numeric_limits<float>::infinity();
        yMin = std::numeric_limits<float>::infinity();
        yMax = -std::numeric_limits<float>::infinity();

        for (auto& entry : curves)
        {
            const auto timeRange = entry->GetTimeRange();
            const auto valueRange = entry->CalculateRange();

            xMin = std::min(xMin, timeRange.first);
            xMax = std::max(xMax, timeRange.second);
            yMin = std::min(yMin, valueRange.first);
            yMax = std::max(yMax, valueRange.second);
        }

        if (xMin == std::numeric_limits<float>::infinity())
            xMin = 0.0f;

        if (xMax == -std::numeric_limits<float>::infinity())
            xMax = 0.0f;

        if (yMin == std::numeric_limits<float>::infinity())
            yMin = 0.0f;

        if (yMax == -std::numeric_limits<float>::infinity())
            yMax = 0.0f;
    }

    template<class T>
    TAnimationCurve<T> AnimationUtility::OffsetCurve(const TAnimationCurve<T>& curve, float offset)
    {
        INT32 numKeys = (INT32)curve.GetNumKeyFrames();

        Vector<TKeyframe<T>> newKeyframes(numKeys);
        for (INT32 i = 0; i < numKeys; i++)
        {
            const TKeyframe<T>& key = curve.GetKeyFrame(i);
            newKeyframes[i].TimeInSpline = key.TimeInSpline + offset;
            newKeyframes[i].Value = key.Value;
        }

        return TAnimationCurve<T>(newKeyframes);
    }

    template TE_CORE_EXPORT TAnimationCurve<Vector3> AnimationUtility::OffsetCurve(const TAnimationCurve<Vector3>& curve, float offset);
    template TE_CORE_EXPORT TAnimationCurve<Vector2> AnimationUtility::OffsetCurve(const TAnimationCurve<Vector2>& curve, float offset);
    template TE_CORE_EXPORT TAnimationCurve<Quaternion> AnimationUtility::OffsetCurve(const TAnimationCurve<Quaternion>& curve, float offset);
    template TE_CORE_EXPORT TAnimationCurve<float> AnimationUtility::OffsetCurve(const TAnimationCurve<float>& curve, float offset);
}
