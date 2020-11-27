#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Threading/TeThreading.h"
#include <climits>

namespace te
{
    /**
     * A memory allocator that allocates elements of the same size. Allows for fairly quick allocations and deallocations.
     *
     * @tparam	ElemSize		Size of a single element in the pool. This will be the exact allocation size. 4 byte minimum.
     * @tparam	ElemsPerBlock	Determines how much space to reserve for elements. This determines the initial size of the
     *							pool, and the additional size the pool will be expanded by every time the number of elements
     *							goes over the available storage limit.
     * @tparam	Alignment		Memory alignment of each allocated element. Note that alignments that are larger than
     *							element size, or aren't a multiplier of element size will introduce additionally padding
     *							for each element, and therefore require more internal memory.
     * @tparam	Lock			If true the pool allocator will be made thread safe (at the cost of performance).
     */
    template <int ElemSize, int ElemsPerBlock = 512, int Alignment = 4, bool Lock = false>
    class PoolAllocator
    {
    private:
        /** A single block able to hold ElemsPerBlock elements. */
        class MemBlock
        {
        public:
            MemBlock(UINT8* blockData)
                : BlockData(blockData)
                , FreePtr(0)
                , FreeElems(ElemsPerBlock)
                , NextBlock(nullptr)
            {
                UINT32 offset = 0;
                for (UINT32 i = 0; i < ElemsPerBlock; i++)
                {
                    UINT32* entryPtr = (UINT32*)&blockData[offset];

                    offset += ActualElemSize;
                    *entryPtr = offset;
                }
            }

            ~MemBlock()
            {
                if (FreeElems != ElemsPerBlock)
                {
#if TE_DEBUG_MODE == 1
                    assert(FreeElems == ElemsPerBlock && "Not all elements were deallocated from a block.");
#endif
                }
            }

            /**
             * Returns the first free address and increments the free pointer. Caller needs to ensure the remaining block
             * size is adequate before calling.
             */
            UINT8* Allocate()
            {
                UINT8* freeEntry = &BlockData[FreePtr];
                FreePtr = *(UINT32*)freeEntry;
                --FreeElems;

                return freeEntry;
            }

            /** Deallocates the provided pointer. */
            void Deallocate(void* data)
            {
                UINT32* entryPtr = (UINT32*)data;
                *entryPtr = FreePtr;
                ++FreeElems;

                FreePtr = (UINT32)(((UINT8*)data) - BlockData);
            }

            UINT8* BlockData;
            UINT32 FreePtr;
            UINT32 FreeElems;
            MemBlock* NextBlock;
        };

    public:
        PoolAllocator()
        {
            static_assert(ElemSize >= 4, "Pool allocator minimum allowed element size is 4 bytes.");
            static_assert(ElemsPerBlock > 0, "Number of elements per block must be at least 1.");
            static_assert(ElemsPerBlock * ActualElemSize <= UINT_MAX, "Pool allocator block size too large.");
        }

        ~PoolAllocator()
        {
            ScopedLock<Lock> lock(_lockPolicy);

            MemBlock* curBlock = _freeBlock;
            while (curBlock != nullptr)
            {
                MemBlock* nextBlock = curBlock->NextBlock;
                DeallocateBlock(curBlock);

                curBlock = nextBlock;
            }
        }

        /** Allocates enough memory for a single element in the pool. */
        UINT8* Allocate()
        {
            ScopedLock<Lock> lock(_lockPolicy);

            if (_freeBlock == nullptr || _freeBlock->FreeElems == 0)
                AllocateBlock();

            _totalNumElems++;
            UINT8* output = _freeBlock->Allocate();

            return output;
        }

        /** Deallocates an element from the pool. */
        void Free(void* data)
        {
            ScopedLock<Lock> lock(_lockPolicy);

            MemBlock* curBlock = _freeBlock;
            while (curBlock)
            {
                constexpr UINT32 blockDataSize = ActualElemSize * ElemsPerBlock;
                if (data >= curBlock->BlockData && data < (curBlock->BlockData + blockDataSize))
                {
                    curBlock->Deallocate(data);
                    _totalNumElems--;

                    if (curBlock->FreeElems == 0 && curBlock->NextBlock)
                    {
                        // Free the block, but only if there is some extra free space in other blocks
                        const UINT32 totalSpace = (_numBlocks - 1) * ElemsPerBlock;
                        const UINT32 freeSpace = totalSpace - _totalNumElems;

                        if (freeSpace > ElemsPerBlock / 2)
                        {
                            _freeBlock = curBlock->NextBlock;
                            DeallocateBlock(curBlock);
                        }
                    }

                    return;
                }

                curBlock = curBlock->NextBlock;
            }

            assert(false);
        }

        /** Allocates and constructs a single pool element. */
        template<class T, class... Args>
        T* Construct(Args &&...args)
        {
            T* data = (T*)Allocate();
            new ((void*)data) T(std::forward<Args>(args)...);

            return data;
        }

        /** Destructs and deallocates a single pool element. */
        template<class T>
        void Destruct(T* data)
        {
            data->~T();
            Free(data);
        }
    
    private:
        /** Allocates a new block of memory using a heap allocator. */
        MemBlock* AllocateBlock()
        {
            MemBlock* newBlock = nullptr;
            MemBlock* curBlock = _freeBlock;

            while (curBlock != nullptr)
            {
                MemBlock* nextBlock = curBlock->NextBlock;
                if (nextBlock != nullptr && nextBlock->FreeElems > 0)
                {
                    // Found an existing block with free space
                    newBlock = nextBlock;

                    curBlock->NextBlock = newBlock->NextBlock;
                    newBlock->NextBlock = _freeBlock;

                    break;
                }

                curBlock = nextBlock;
            }

            if (newBlock == nullptr)
            {
                constexpr UINT32 blockDataSize = ActualElemSize * ElemsPerBlock;
                size_t paddedBlockDataSize = blockDataSize + (Alignment - 1); // Padding for potential alignment correction

                UINT8* data = (UINT8*)te_allocate(sizeof(MemBlock) + (UINT32)paddedBlockDataSize);

                void* blockData = data + sizeof(MemBlock);
                blockData = std::align(Alignment, blockDataSize, blockData, paddedBlockDataSize);

                newBlock = new (data) MemBlock((UINT8*)blockData);
                _numBlocks++;

                newBlock->NextBlock = _freeBlock;
            }

            _freeBlock = newBlock;
            return newBlock;
        }

        /** Deallocates a block of memory. */
        void DeallocateBlock(MemBlock* block)
        {
            block->~MemBlock();
            te_free(block);

            _numBlocks--;
        }

        static constexpr int ActualElemSize = ((ElemSize + Alignment - 1) / Alignment) * Alignment;

        LockingPolicy<Lock> _lockPolicy;
        MemBlock* _freeBlock = nullptr;
        UINT32 _totalNumElems = 0;
        UINT32 _numBlocks = 0;
    };

    /**
     * Helper class used by GlobalPoolAlloc that allocates a static pool allocator. GlobalPoolAlloc cannot do it
     * directly since it gets specialized which means the static members would need to be defined in the implementation
     * file, which complicates its usage.
     */
    template <class T, int ElemsPerBlock = 512, int Alignment = 4, bool Lock = true>
    class StaticPoolAllocator
    {
    public:
        static PoolAllocator<sizeof(T), ElemsPerBlock, Alignment, Lock> m;
    };

    template <class T, int ElemsPerBlock, int Alignment, bool Lock>
    PoolAllocator<sizeof(T), ElemsPerBlock, Alignment, Lock> StaticPoolAllocator<T, ElemsPerBlock, Alignment, Lock>::m;

    /** Specializable template that allows users to implement globally accessible pool allocators for custom types. */
    template<class T>
    class GlobalPoolAllocator : std::false_type
    {
        template <typename T2>
        struct AlwaysFalse : std::false_type { };

        static_assert(AlwaysFalse<T>::value, "No global pool allocator exists for the type.");
    };

    /**
     * Implements a global pool for the specified type. The pool will initially have enough room for ElemsPerBlock and
     * will grow by that amount when exceeded. Global pools are thread safe by default.
     */
#define IMPLEMENT_GLOBAL_POOL(Type, ElemsPerBlock)									\
	template<> class GlobalPoolAllocator<Type> : public StaticPoolAllocator<Type, ElemsPerBlock> { };

     /** Allocates a new object of type T using the global pool allocator, without constructing it. */
    template<class T>
    T* te_pool_allocate()
    {
        return (T*)GlobalPoolAllocator<T>::m.Allocate();
    }

    /** Allocates and constructs a new object of type T using the global pool allocator. */
    template<class T, class... Args>
    T* te_pool_new(Args &&...args)
    {
        T* data = te_pool_allocate<T>();
        new ((void*)data) T(std::forward<Args>(args)...);

        return data;
    }

    /** Frees the provided object using its global pool allocator, without destructing it. */
    template<class T>
    void te_pool_free(T* ptr)
    {
        GlobalPoolAllocator<T>::m.Free(ptr);
    }

    /** Frees and destructs the provided object using its global pool allocator. */
    template<class T>
    void te_pool_delete(T* ptr)
    {
        ptr->~T();
        te_pool_free(ptr);
    }
}
