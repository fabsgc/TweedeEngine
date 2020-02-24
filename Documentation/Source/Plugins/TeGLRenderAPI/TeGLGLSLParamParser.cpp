#include "TeGLGLSLParamParser.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeGpuParamDesc.h"
#include "Math/TeMath.h"

namespace te
{
    UINT32 GLGLSLParamParser::CalcInterfaceBlockElementSizeAndOffset(GpuParamDataType type, UINT32 arraySize, UINT32& offset)
    {
        const GpuParamDataTypeInfo& typeInfo = te::GpuParams::PARAM_SIZES.lookup[type];
        UINT32 size = (typeInfo.baseTypeSize * typeInfo.numColumns * typeInfo.numRows) / 4;
        UINT32 alignment = typeInfo.alignment / 4;

        // Fix alignment if needed
        UINT32 alignOffset = offset % alignment;
        if (alignOffset != 0)
        {
            UINT32 padding = (alignment - alignOffset);
            offset += padding;
        }

        if (arraySize > 1)
        {
            // Array elements are always padded and aligned to vec4
            alignOffset = size % 4;
            if (alignOffset != 0)
            {
                UINT32 padding = (4 - alignOffset);
                size += padding;
            }

            alignOffset = offset % 4;
            if (alignOffset != 0)
            {
                UINT32 padding = (4 - alignOffset);
                offset += padding;
            }

            return size;
        }
        else
            return size;
    }
}
