#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeVertexDeclaration.h"

namespace te 
{
    /**	Handles parsing of GPU program microcode and extracting constant and input parameter data. */
    class GLGLSLParamParser
    {
    public:
        /**
         * Calculates the size and alignment of a single element within a shader interface block using the std140 layout.
         *
         * @param[in]		type		Type of the element. Structs are not supported.
         * @param[in]		arraySize	Number of array elements of the element (1 if it's not an array).
         * @param[in, out]	offset		Current location in some parent buffer at which the element should be placed at. If the
         *								location doesn't match the element's alignment, the value will be modified to a valid
         *								alignment. In multiples of 4 bytes.
         * @return						Size of the element, in multiples of 4 bytes.
         */
        static UINT32 CalcInterfaceBlockElementSizeAndOffset(GpuParamDataType type, UINT32 arraySize, UINT32& offset);

    private:
        /** Types of GLSL parameters. */
        enum class ParamType
        {
            UniformBlock,
            Texture,
            Sampler,
            Image,
            StorageBlock,
            Count // Keep at end
        };
    };
}
