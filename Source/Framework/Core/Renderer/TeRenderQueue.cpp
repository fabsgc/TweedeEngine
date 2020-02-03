#include "Renderer/TeRenderQueue.h"
#include "RenderAPI/TeSubMesh.h"
#include "Mesh/TeMesh.h"
#include "Material/TeMaterial.h"
#include "Material/TeShader.h"
#include "Renderer/TeRenderElement.h"

namespace te
{ 
    RenderQueue::RenderQueue(StateReduction mode)
        : _stateReductionMode(mode)
    { }

    RenderQueue::~RenderQueue()
    { }

    void RenderQueue::Add(const RenderElement* element, float distFromCamera)
    {
        SPtr<Material> material = element->MaterialElem;
        SPtr<Shader> shader = material->GetShader();

        UINT32 queuePriority = shader->GetQueuePriority();
        QueueSortType sortType = shader->GetQueueSortType();
        UINT32 shaderId = shader->GetId();
        //bool separablePasses = shader->GetAllowSeparablePasses();

        switch (sortType)
        {
        case QueueSortType::None:
            distFromCamera = 0;
            break;
        case QueueSortType::BackToFront:
            distFromCamera = -distFromCamera;
            break;
        case QueueSortType::FrontToBack:
            break;
        }

        // TODO passes

        UINT32 idx = (UINT32)_sortableElementIdx.size();
        _sortableElementIdx.push_back(idx);

        _sortableElements.push_back(SortableElement());
        SortableElement& sortableElem = _sortableElements.back();

        sortableElem.SeqIdx = idx;
        sortableElem.Priority = queuePriority;
        sortableElem.ShaderId = shaderId;
        //sortableElem.TechniqueIdx = techniqueIdx; TODO
        //sortableElem.PassIdx = i; TODO
        sortableElem.DistFromCamera = distFromCamera;

        _elements.push_back(element);
    }

    void RenderQueue::Sort()
    {
        std::function<bool(UINT32, UINT32, const Vector<SortableElement>&)> sortMethod;

        switch (_stateReductionMode)
        {
        case StateReduction::None:
            sortMethod = &ElementSorterNoGroup;
            break;
        case StateReduction::Material:
            sortMethod = &ElementSorterPreferGroup;
            break;
        case StateReduction::Distance:
            sortMethod = &ElementSorterPreferDistance;
            break;
        }

        // TODO
        for (auto& element : _elements)
        {
            _sortedRenderElements.push_back(RenderQueueElement());

            RenderQueueElement& sortedElem = _sortedRenderElements.back();
            sortedElem.RenderElem = element;
        }
    }

    void RenderQueue::Clear()
    {
        _sortableElements.clear();
        _sortableElementIdx.clear();
        _elements.clear();
        _sortedRenderElements.clear();
    }

    bool RenderQueue::ElementSorterNoGroup(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
    {
        // TODO
        return false;
    }

    bool RenderQueue::ElementSorterPreferGroup(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
    {
        // TODO
        return false;
    }

    bool RenderQueue::ElementSorterPreferDistance(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
    {
        // TODO
        return false;
    }

    const Vector<RenderQueueElement>& RenderQueue::GetSortedElements() const
    {
        return _sortedRenderElements;
    }
}
