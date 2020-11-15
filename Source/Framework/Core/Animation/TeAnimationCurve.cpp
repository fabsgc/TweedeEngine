#include "Animation/TeAnimationCurve.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2.h"
#include "Math/TeQuaternion.h"
#include "Math/TeMath.h"

namespace te
{
    template <class T>
    TAnimationCurve<T>::TAnimationCurve(const Vector<KeyFrame>& keyframes)
        : _keyframes(keyframes)
    { 
#if TE_DEBUG_MODE
        // Ensure keyframes are sorted
        if(!keyframes.empty())
        {
            float time = keyframes[0].TimeInSpline;
            for (UINT32 i = 1; i < (UINT32)keyframes.size(); i++)
            {
                assert(keyframes[i].TimeInSpline >= time);
                time = keyframes[i].TimeInSpline;
            }
        }
#endif

        if (!keyframes.empty())
            _end = keyframes.back().TimeInSpline;
        else
            _end = 0.0f;

        _start = 0.0f;
        _length = _end;
    }
}
