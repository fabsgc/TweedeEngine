#include "Renderer/TeRenderQueue.h"
#include "RenderAPI/TeSubMesh.h"
#include "Mesh/TeMesh.h"
#include "Material/TeMaterial.h"
#include "Material/TeShader.h"
#include "Renderer/TeRenderElement.h"

using namespace std::placeholders;

namespace te
{ 
    RenderQueue::RenderQueue(StateReduction mode)
        : _stateReductionMode(mode)
    { }

    RenderQueue::~RenderQueue()
    { }

    void RenderQueue::Add(const RenderElement* element, float distFromCamera, UINT32 techniqueIdx)
    {
        SPtr<Material> material = element->MaterialElem;
        SPtr<Shader> shader = material->GetShader();

        UINT32 queuePriority = shader->GetQueuePriority();
        QueueSortType sortType = shader->GetQueueSortType();
        UINT32 shaderId = shader->GetId();
        bool separablePasses = shader->GetAllowSeparablePasses();

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

        UINT32 numPasses = material->GetNumPasses(techniqueIdx);
        if (!separablePasses)
            numPasses = std::min(1U, numPasses);

        for (UINT32 i = 0; i < numPasses; i++)
        {
            UINT32 idx = (UINT32)_sortableElementIdx.size();
            _sortableElementIdx.push_back(idx);

            _sortableElements.push_back(SortableElement());
            SortableElement& sortableElem = _sortableElements.back();

            sortableElem.SeqIdx = idx;
            sortableElem.Priority = queuePriority;
            sortableElem.ShaderId = shaderId;
            sortableElem.TechniqueIdx = techniqueIdx;
            sortableElem.PassIdx = i;
            sortableElem.DistFromCamera = distFromCamera;

            _elements.push_back(element);
        }
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

        // Sort only indices since we generate an entirely new data set anyway, it doesn't make sense to move sortable elements
        std::sort(_sortableElementIdx.begin(), _sortableElementIdx.end(), std::bind(sortMethod, _1, _2, _sortableElements));

        UINT32 prevShaderId = (UINT32)-1;
        UINT32 prevTechniqueIdx = (UINT32)-1;
        UINT32 prevPassIdx = (UINT32)-1;
        for (UINT32 i = 0; i < (UINT32)_sortableElementIdx.size(); i++)
        {
            const UINT32 idx = _sortableElementIdx[i];
            const SortableElement& elem = _sortableElements[idx];
            const RenderElement* renderElem = _elements[idx];

            const bool separablePasses = renderElem->MaterialElem->GetShader()->GetAllowSeparablePasses();

            if (separablePasses)
            {
                _sortedRenderElements.push_back(RenderQueueElement());

                RenderQueueElement& sortedElem = _sortedRenderElements.back();
                sortedElem.RenderElem = renderElem;
                sortedElem.TechniqueIdx = elem.TechniqueIdx;
                sortedElem.PassIdx = elem.PassIdx;

                if (prevShaderId != elem.ShaderId || prevTechniqueIdx != elem.TechniqueIdx || prevPassIdx != elem.PassIdx)
                {
                    sortedElem.ApplyPass = true;
                    prevShaderId = elem.ShaderId;
                    prevTechniqueIdx = elem.TechniqueIdx;
                    prevPassIdx = elem.PassIdx;
                }
                else
                    sortedElem.ApplyPass = false;
            }
            else
            {
                const UINT32 numPasses = renderElem->MaterialElem->GetNumPasses(elem.TechniqueIdx);
                for (UINT32 j = 0; j < numPasses; j++)
                {
                    _sortedRenderElements.push_back(RenderQueueElement());

                    RenderQueueElement& sortedElem = _sortedRenderElements.back();
                    sortedElem.RenderElem = renderElem;
                    sortedElem.TechniqueIdx = elem.TechniqueIdx;
                    sortedElem.PassIdx = j;

                    if (prevShaderId != elem.ShaderId || prevTechniqueIdx != elem.TechniqueIdx || prevPassIdx != j)
                    {
                        sortedElem.ApplyPass = true;
                        prevShaderId = elem.ShaderId;
                        prevTechniqueIdx = elem.TechniqueIdx;
                        prevPassIdx = j;
                    }
                    else
                        sortedElem.ApplyPass = false;
                }
            }
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
        const SortableElement& a = lookup[aIdx];
        const SortableElement& b = lookup[bIdx];

        UINT8 isHigher = (a.Priority > b.Priority) << 2 |
            (a.DistFromCamera < b.DistFromCamera) << 1 |
            (a.SeqIdx < b.SeqIdx);

        UINT8 isLower = (a.Priority < b.Priority) << 2 |
            (a.DistFromCamera > b.DistFromCamera) << 1 |
            (a.SeqIdx > b.SeqIdx);

        return isHigher > isLower;
    }

    bool RenderQueue::ElementSorterPreferGroup(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
    {
        const SortableElement& a = lookup[aIdx];
        const SortableElement& b = lookup[bIdx];

        UINT8 isHigher = (a.Priority > b.Priority) << 5 |
            (a.ShaderId < b.ShaderId) << 4 |
            (a.TechniqueIdx < b.TechniqueIdx) << 3 |
            (a.PassIdx < b.PassIdx) << 2 |
            (a.DistFromCamera < b.DistFromCamera) << 1 |
            (a.SeqIdx < b.SeqIdx);

        UINT8 isLower = (a.Priority < b.Priority) << 5 |
            (a.ShaderId > b.ShaderId) << 4 |
            (a.TechniqueIdx > b.TechniqueIdx) << 3 |
            (a.PassIdx > b.PassIdx) << 2 |
            (a.DistFromCamera > b.DistFromCamera) << 1 |
            (a.SeqIdx > b.SeqIdx);

        return isHigher > isLower;
    }

    bool RenderQueue::ElementSorterPreferDistance(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
    {
        const SortableElement& a = lookup[aIdx];
        const SortableElement& b = lookup[bIdx];

        UINT8 isHigher = (a.Priority > b.Priority) << 5 |
            (a.DistFromCamera < b.DistFromCamera) << 4 |
            (a.ShaderId < b.ShaderId) << 3 |
            (a.TechniqueIdx < b.TechniqueIdx) << 2 |
            (a.PassIdx < b.PassIdx) << 1 |
            (a.SeqIdx < b.SeqIdx);

        UINT8 isLower = (a.Priority < b.Priority) << 5 |
            (a.DistFromCamera > b.DistFromCamera) << 4 |
            (a.ShaderId > b.ShaderId) << 3 |
            (a.TechniqueIdx > b.TechniqueIdx) << 2 |
            (a.PassIdx > b.PassIdx) << 1 |
            (a.SeqIdx > b.SeqIdx);

        return isHigher > isLower;
    }

    const Vector<RenderQueueElement>& RenderQueue::GetSortedElements() const
    {
        return _sortedRenderElements;
    }
}
