#include "Prerequisites/PrerequisitesUtil.h"
#include "StackAllocator.h"

namespace te
{
    StackAllocator* _GlobalStackAllocator = nullptr;

    StackAllocator& gStackAllocator()
    {
        if (_GlobalStackAllocator == nullptr)
        {
            // Note: This will leak memory but since it should exist throughout the entirety 
            // of runtime it should only leak on shutdown when the OS will free it anyway.
            _GlobalStackAllocator = new StackAllocator();
        }

        return *_GlobalStackAllocator;
    }

    void* te_stack_allocate(UINT32 numBytes)
    {
        return gStackAllocator().Allocate(numBytes);
    }

    void te_stack_deallocate(void* data)
    {
        gStackAllocator().Deallocate((UINT8*)data);
    }
}