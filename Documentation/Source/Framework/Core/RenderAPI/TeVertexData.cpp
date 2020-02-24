#include "RenderAPI/TeVertexData.h"
#include "RenderAPI/TeHardwareBufferManager.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "Math/TeVector3.h"
#include "RenderAPI/TeRenderAPI.h"

namespace te
{
    void VertexData::SetBuffer(UINT32 index, SPtr<VertexBuffer> buffer)
    {
        _vertexBuffers[index] = buffer;
        RecalculateMaxIndex();
    }

    SPtr<VertexBuffer> VertexData::GetBuffer(UINT32 index) const
    {
        auto iterFind = _vertexBuffers.find(index);
        if(iterFind != _vertexBuffers.end())
        {
            return iterFind->second;
        }

        return nullptr;
    }

    bool VertexData::IsBufferBound(UINT32 index) const
    {
        auto iterFind = _vertexBuffers.find(index);
        if(iterFind != _vertexBuffers.end())
        {
            if(iterFind->second != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    void VertexData::RecalculateMaxIndex()
    {
        _maxBufferIdx = 0;
        for (auto& bufferData : _vertexBuffers)
        {
            _maxBufferIdx = std::max(bufferData.first, _maxBufferIdx);
        }
    }
}
