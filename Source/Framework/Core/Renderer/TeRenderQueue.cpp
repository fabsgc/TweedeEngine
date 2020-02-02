#include "Renderer/TeRenderQueue.h"
#include "RenderAPI/TeSubMesh.h"
#include "Mesh/TeMesh.h"

namespace te
{ 
    RenderQueue::RenderQueue(StateReduction mode)
        : _stateReductionMode(mode)
    { }

    RenderQueue::~RenderQueue()
    { }

    void RenderQueue::Add(const RenderElement* element, float distFromCamera)
    { }

    void RenderQueue::Sort()
    { }

    void RenderQueue::Clear()
    {
        _sortableElements.clear();
        _sortableElementIdx.clear();
        _elements.clear();
        _sortedRenderElements.clear();
    }

    bool RenderQueue::ElementSorterPreferDistance(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
    {
        return false;
    }

    const Vector<RenderQueueElement>& RenderQueue::GetSortedElements() const
    {
        return _sortedRenderElements;
    }
}
