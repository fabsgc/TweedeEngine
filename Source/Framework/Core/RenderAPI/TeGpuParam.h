#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Math/TeMatrix3.h"
#include "Math/TeMatrix4.h"
#include "Math/TeMatrixNxM.h"
#include "Math/TeVector3I.h"
#include "Math/TeVector4I.h"
#include "Utility/TeColor.h"

namespace te
{
    /**
     * Policy class that allows us to re-use this template class for matrices which might need transposing, and other
     * types which do not. Matrix needs to be transposed for certain render systems depending on how they store them
     * in memory.
     */
    template<class Type>
    struct TransposePolicy
    {
        static Type Transpose(const Type& value) { return value; }
        static bool TansposeEnabled(bool enabled) { return false; }
    };

    /** Transpose policy for 3x3 matrix. */
    template<>
    struct TransposePolicy<Matrix3>
    {
        static Matrix3 Transpose(const Matrix3& value) { return value.Transpose(); }
        static bool TransposeEnabled(bool enabled) { return enabled; }
    };

    /** Transpose policy for 4x4 matrix. */
    template<>
    struct TransposePolicy<Matrix4>
    {
        static Matrix4 Transpose(const Matrix4& value) { return value.Transpose(); }
        static bool TransposeEnabled(bool enabled) { return enabled; }
    };

    /** Transpose policy for NxM matrix. */
    template<int N, int M>
    struct TransposePolicy<MatrixNxM<N, M>>
    {
        static MatrixNxM<M, N> Transpose(const MatrixNxM<N, M>& value) { return value.Transpose(); }
        static bool TransposeEnabled(bool enabled) { return enabled; }
    };
}
