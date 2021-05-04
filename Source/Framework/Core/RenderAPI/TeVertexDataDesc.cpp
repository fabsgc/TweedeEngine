#include "TeVertexDataDesc.h"
#include "RenderAPI/TeHardwareBufferManager.h"

namespace te
{ 
    void VertexDataDesc::AddVertElem(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx,
        UINT32 streamIdx, UINT32 instanceStepRate)
    {
        ClearIfItExists(type, semantic, semanticIdx, streamIdx);

        VertexElement newElement(streamIdx, 0, type, semantic, semanticIdx, instanceStepRate);

        // Insert it so it is sorted by stream
        UINT32 insertToIndex = (UINT32)_vertexElements.size();
        UINT32 idx = 0;
        for (auto& elem : _vertexElements)
        {
            if (elem.GetStreamIdx() > streamIdx)
            {
                insertToIndex = idx;
                break;
            }

            idx++;
        }

        _vertexElements.insert(_vertexElements.begin() + insertToIndex, newElement);
    }

    Vector<VertexElement> VertexDataDesc::CreateElements() const
    {
        UINT32 maxStreamIdx = GetMaxStreamIdx();

        UINT32 numStreams = maxStreamIdx + 1;
        UINT32* streamOffsets = te_newN<UINT32>(numStreams);
        for (UINT32 i = 0; i < numStreams; i++)
            streamOffsets[i] = 0;

        Vector<VertexElement> declarationElements;
        for (auto& vertElem : _vertexElements)
        {
            UINT32 streamIdx = vertElem.GetStreamIdx();

            declarationElements.push_back(VertexElement(streamIdx, streamOffsets[streamIdx], vertElem.GetType(),
                vertElem.GetSemantic(), vertElem.GetSemanticIdx(), vertElem.GetInstanceStepRate()));

            streamOffsets[streamIdx] += vertElem.GetSize();
        }

        te_deleteN(streamOffsets, numStreams);

        return declarationElements;
    }

    UINT32 VertexDataDesc::GetMaxStreamIdx() const
    {
        UINT32 maxStreamIdx = 0;
        UINT32 numElems = (UINT32)_vertexElements.size();
        for (UINT32 i = 0; i < numElems; i++)
        {
            for (auto& vertElem : _vertexElements)
            {
                maxStreamIdx = std::max((UINT32)maxStreamIdx, (UINT32)vertElem.GetStreamIdx());
            }
        }

        return maxStreamIdx;
    }

    bool VertexDataDesc::HasStream(UINT32 streamIdx) const
    {
        for (auto& vertElem : _vertexElements)
        {
            if (vertElem.GetStreamIdx() == streamIdx)
                return true;
        }

        return false;
    }

    bool VertexDataDesc::HasElement(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
    {
        auto findIter = std::find_if(_vertexElements.begin(), _vertexElements.end(),
            [semantic, semanticIdx, streamIdx](const VertexElement& x)
            {
                return x.GetSemantic() == semantic && x.GetSemanticIdx() == semanticIdx && x.GetStreamIdx() == streamIdx;
            });

        if (findIter != _vertexElements.end())
            return true;

        return false;
    }

    UINT32 VertexDataDesc::GetElementSize(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
    {
        for (auto& element : _vertexElements)
        {
            if (element.GetSemantic() == semantic && element.GetSemanticIdx() == semanticIdx && element.GetStreamIdx() == streamIdx)
                return element.GetSize();
        }

        return static_cast<UINT32>(-1);
    }

    UINT32 VertexDataDesc::GetElementOffsetFromStream(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
    {
        UINT32 vertexOffset = 0;
        for (auto& element : _vertexElements)
        {
            if (element.GetStreamIdx() != streamIdx)
                continue;

            if (element.GetSemantic() == semantic && element.GetSemanticIdx() == semanticIdx)
                break;

            vertexOffset += element.GetSize();
        }

        return vertexOffset;
    }

    UINT32 VertexDataDesc::GetVertexStride(UINT32 streamIdx) const
    {
        UINT32 vertexStride = 0;
        for (auto& element : _vertexElements)
        {
            if (element.GetStreamIdx() == streamIdx)
                vertexStride += element.GetSize();
        }

        return vertexStride;
    }

    UINT32 VertexDataDesc::GetVertexStride() const
    {
        UINT32 vertexStride = 0;
        for (auto& element : _vertexElements)
        {
            vertexStride += element.GetSize();
        }

        return vertexStride;
    }

    UINT32 VertexDataDesc::GetStreamOffset(UINT32 streamIdx) const
    {
        UINT32 streamOffset = 0;
        for (auto& element : _vertexElements)
        {
            if (element.GetStreamIdx() == streamIdx)
                break;

            streamOffset += element.GetSize();
        }

        return streamOffset;
    }

    const VertexElement* VertexDataDesc::GetElement(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
    {
        auto findIter = std::find_if(_vertexElements.begin(), _vertexElements.end(),
            [semantic, semanticIdx, streamIdx](const VertexElement& x)
            {
                return x.GetSemantic() == semantic && x.GetSemanticIdx() == semanticIdx && x.GetStreamIdx() == streamIdx;
            }
        );

        if (findIter != _vertexElements.end())
            return &(*findIter);

        return nullptr;
    }

    void VertexDataDesc::ClearIfItExists(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
    {
        auto findIter = std::find_if(_vertexElements.begin(), _vertexElements.end(),
            [semantic, semanticIdx, streamIdx](const VertexElement& x)
            {
                return x.GetSemantic() == semantic && x.GetSemanticIdx() == semanticIdx && x.GetStreamIdx() == streamIdx;
            }
        );

        if (findIter != _vertexElements.end())
            _vertexElements.erase(findIter);
    }

    SPtr<VertexDataDesc> VertexDataDesc::Create()
    {
        return te_shared_ptr_new<VertexDataDesc>();
    }
}
