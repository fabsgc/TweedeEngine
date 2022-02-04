#pragma once

#include "TeCorePrerequisites.h"

namespace te 
{
    class RendererView;

    enum class StateReduction
    {
        Never, /** Do not sort at all, reduces CPU cost */
        None, /**< No grouping based on material will be done. */
        Material, /**< Elements will be grouped by material first, by distance second. */
        Distance /**< Elements will be grouped by distance first, material second. */
    };

    /** Contains data needed for performing a single rendering pass. */
    struct RenderQueueElement
    { 
        const RenderElement* RenderElem = nullptr;
        UINT32 PassIdx = 0;
        UINT32 TechniqueIdx = 0;
        bool ApplyPass = true;
    };

    /**
     * Render objects determines rendering order of objects contained within it. Rendering order is determined by object
     * material, and can influence rendering of transparent or opaque objects, or be used to improve performance by grouping
     * similar objects together.
     */
    class TE_CORE_EXPORT RenderQueue : public NonCopyable
    {
        /**	Data used for renderable element sorting. Represents a single pass for a single mesh. */
        struct SortableElement
        {
            UINT32 SeqIdx;
            INT32 Priority;
            float DistFromCamera;
            UINT32 ShaderId;
            UINT32 TechniqueIdx;
            UINT32 PassIdx;
            UINT32 MaterialId;
        };

    public:
        RenderQueue(StateReduction grouping = StateReduction::Distance);
        virtual ~RenderQueue();

        /**
         * Adds a new entry to the render queue.
         *
         * @param[in]	element			Renderable element to add to the queue.
         * @param[in]	distFromCamera	Distance of this object from the camera. Used for distance sorting.
         * @param[in]	techniqueIdx	Index of the technique within @p element's material that's to be used to render the element with.
         */
        void Add(const RenderElement* element, float distFromCamera, UINT32 techniqueIdx);
        void Sort();
        void Clear();

        /** Returns a list of sorted render elements. Caller must ensure sort() is called before this method. */
        const Vector<RenderQueueElement>& GetSortedElements() const;

        /**
         * Controls if and how a render queue groups renderable objects by material in order to reduce number of state
         * changes.
         */
        void SetStateReduction(StateReduction mode) { _stateReductionMode = mode; }

    protected:
        /**	Callback used for sorting elements with no material grouping. */
        static bool ElementSorterNoGroup(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup);

        /**	Callback used for sorting elements with preferred material grouping. */
        static bool ElementSorterPreferGroup(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup);

        /**	Callback used for sorting elements with material grouping after sorting. */
        static bool ElementSorterPreferDistance(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup);

    protected:
        Vector<SortableElement> _sortableElements;
        Vector<UINT32> _sortableElementIdx;
        Vector<const RenderElement*> _elements;

        Vector<RenderQueueElement> _sortedRenderElements;
        StateReduction _stateReductionMode;
    };
}