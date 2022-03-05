#include "Prerequisites/TePrerequisitesUtility.h"
#include "Utility/TeFrameAllocator.h"

namespace te
{
    UINT8* FrameAllocator::MemBlock::Allocate(UINT32 amount)
    {
        UINT8* freePtr = &_data[_freePtr];
        _freePtr += amount;

        return freePtr;
    }

    void FrameAllocator::MemBlock::Clear()
    {
        _freePtr = 0;
    }

    FrameAllocator::FrameAllocator(UINT32 blockSize)
        : _blockSize(blockSize)
        , _freeBlock(nullptr)
        , _nextBlockIdx(0)
        , _totalAllocBytes(0)
        , _lastFrame(nullptr)
    {
    }

    FrameAllocator::~FrameAllocator()
    {
        for (auto& block : _blocks)
            DeallocateBlock(block);
    }

    UINT8* FrameAllocator::Allocate(UINT32 amount)
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        amount += sizeof(UINT32);
#endif
        UINT32 freeMem = 0;
        if (_freeBlock != nullptr)
            freeMem = _freeBlock->_size - _freeBlock->_freePtr;

        if (amount > freeMem)
            AllocateBlock(amount);

        UINT8* data = _freeBlock->Allocate(amount);

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        _totalAllocBytes += amount;

        UINT32* storedSize = reinterpret_cast<UINT32*>(data);
        *storedSize = amount;

        return data + sizeof(UINT32);
#else
        return data;
#endif
    }

    UINT8* FrameAllocator::AllocateAligned(UINT32 amount, UINT32 alignment)
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        amount += sizeof(UINT32);
#endif

        UINT32 freeMem = 0;
        UINT32 freePtr = 0;
        if (_freeBlock != nullptr)
        {
            freeMem = _freeBlock->_size - _freeBlock->_freePtr;

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            freePtr = _freeBlock->_freePtr + sizeof(UINT32);
#else
            freePtr = _freeBlock->_freePtr;
#endif
        }

        UINT32 alignOffset = (alignment - (freePtr & (alignment - 1))) & (alignment - 1);
        if ((amount + alignOffset) > freeMem)
        {
            // New blocks are allocated on a 16 byte boundary, ensure enough space is allocated taking into account
            // the requested alignment

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            alignOffset = (alignment - (sizeof(UINT32) & (alignment - 1))) & (alignment - 1);
#else
            if (alignment > 16)
                alignOffset = alignment - 16;
            else
                alignOffset = 0;
#endif

            AllocateBlock(amount + alignOffset);
        }

        amount += alignOffset;
        UINT8* data = _freeBlock->Allocate(amount);

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        _totalAllocBytes += amount;

        UINT32* storedSize = reinterpret_cast<UINT32*>(data + alignOffset);
        *storedSize = amount;

        return data + sizeof(UINT32) + alignOffset;
#else
        return data + alignOffset;
#endif
    }

    void FrameAllocator::Free(UINT8* data)
    {
        // Dealloc is only used for debug and can be removed if needed. All the actual deallocation
        // happens in clear()

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if (data)
        {
            data -= sizeof(UINT32);
            UINT32* storedSize = reinterpret_cast<UINT32*>(data);
            _totalAllocBytes -= *storedSize;
        }
#endif
    }

    void FrameAllocator::MarkFrame()
    {
        void** framePtr = (void**)Allocate(sizeof(void*));
        *framePtr = _lastFrame;
        _lastFrame = framePtr;
    }

    void FrameAllocator::Clear()
    {
        if (_lastFrame != nullptr)
        {
            assert(_blocks.size() > 0 && _nextBlockIdx > 0);

            Free((UINT8*)_lastFrame);

            UINT8* framePtr = (UINT8*)_lastFrame;
            _lastFrame = *(void**)_lastFrame;

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            framePtr -= sizeof(UINT32);
#endif

            UINT32 startBlockIdx = _nextBlockIdx - 1;
            UINT32 numFreedBlocks = 0;
            for (INT32 i = startBlockIdx; i >= 0; i--)
            {
                MemBlock* curBlock = _blocks[i];
                UINT8* blockEnd = curBlock->_data + curBlock->_size;
                if (framePtr >= curBlock->_data && framePtr < blockEnd)
                {
                    UINT8* dataEnd = curBlock->_data + curBlock->_freePtr;
                    UINT32 sizeInBlock = (UINT32)(dataEnd - framePtr);
                    assert(sizeInBlock <= curBlock->_freePtr);

                    curBlock->_freePtr -= sizeInBlock;
                    if (curBlock->_freePtr == 0)
                    {
                        numFreedBlocks++;

                        // Reset block counter if we're gonna reallocate this one
                        if (numFreedBlocks > 1)
                            _nextBlockIdx = (UINT32)i;
                    }

                    break;
                }
                else
                {
                    curBlock->_freePtr = 0;
                    _nextBlockIdx = (UINT32)i;
                    numFreedBlocks++;
                }
            }

            if (numFreedBlocks > 1)
            {
                UINT32 totalBytes = 0;
                for (UINT32 i = 0; i < numFreedBlocks; i++)
                {
                    MemBlock* curBlock = _blocks[_nextBlockIdx];
                    totalBytes += curBlock->_size;

                    DeallocateBlock(curBlock);
                    _blocks.erase(_blocks.begin() + _nextBlockIdx);
                }

                UINT32 oldNextBlockIdx = _nextBlockIdx;
                AllocateBlock(totalBytes);

                // Point to the first non-full block, or if none available then point the the block we just allocated
                if (oldNextBlockIdx > 0)
                    _freeBlock = _blocks[oldNextBlockIdx - 1];
            }
            else
            {
                _freeBlock = _blocks[_nextBlockIdx - 1];
            }
        }
        else
        {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            if (_totalAllocBytes.load() > 0)
                TE_ASSERT_ERROR(false, "Not all frame allocated bytes were properly released.");
#endif

            if (_blocks.size() > 1)
            {
                // Merge all blocks into one
                UINT32 totalBytes = 0;
                for (auto& block : _blocks)
                {
                    totalBytes += block->_size;
                    DeallocateBlock(block);
                }

                _blocks.clear();
                _nextBlockIdx = 0;

                AllocateBlock(totalBytes);
            }
            else if (_blocks.size() > 0)
                _blocks[0]->_freePtr = 0;
        }
    }

    FrameAllocator::MemBlock* FrameAllocator::AllocateBlock(UINT32 wantedSize)
    {
        UINT32 blockSize = _blockSize;
        if (wantedSize > blockSize)
            blockSize = wantedSize;

        MemBlock* newBlock = nullptr;
        while (_nextBlockIdx < _blocks.size())
        {
            MemBlock* curBlock = _blocks[_nextBlockIdx];
            if (blockSize <= curBlock->_size)
            {
                newBlock = curBlock;
                _nextBlockIdx++;
                break;
            }
            else
            {
                // Found an empty block that doesn't fit our data, delete it
                DeallocateBlock(curBlock);
                _blocks.erase(_blocks.begin() + _nextBlockIdx);
            }
        }

        if (newBlock == nullptr)
        {
            UINT32 alignOffset = 16 - (sizeof(MemBlock) & (16 - 1));

            UINT8* data = (UINT8*)reinterpret_cast<UINT8*>(te_allocate_aligned16(blockSize + sizeof(MemBlock) + alignOffset));
            newBlock = new (data) MemBlock(blockSize);
            data += sizeof(MemBlock) + alignOffset;
            newBlock->_data = data;

            _blocks.push_back(newBlock);
            _nextBlockIdx++;
        }

        _freeBlock = newBlock; // If previous block had some empty space it is lost until next "clear"

        return newBlock;
    }

    void FrameAllocator::DeallocateBlock(MemBlock* block)
    {
        block->~MemBlock();
        te_free_aligned16(block);
    }

    TE_THREADLOCAL FrameAllocator* _GlobalFrameAllocator = nullptr;

    TE_UTILITY_EXPORT FrameAllocator& gFrameAllocator()
    {
        if (_GlobalFrameAllocator == nullptr)
        {
            // Note: This will leak memory but since it should exist throughout the entirety
            // of runtime it should only leak on shutdown when the OS will free it anyway.
            _GlobalFrameAllocator = new FrameAllocator();
        }

        return *_GlobalFrameAllocator;
    }

    TE_UTILITY_EXPORT UINT8* te_frame_allocate(UINT32 numBytes)
    {
        return gFrameAllocator().Allocate(numBytes);
    }

    TE_UTILITY_EXPORT UINT8* te_frame_allocate_aligned(UINT32 count, UINT32 align)
    {
        return gFrameAllocator().AllocateAligned(count, align);
    }

    TE_UTILITY_EXPORT void te_frame_free(void* data)
    {
        gFrameAllocator().Free((UINT8*)data);
    }

    TE_UTILITY_EXPORT void te_frame_free_aligned(void* data)
    {
        gFrameAllocator().Free((UINT8*)data);
    }

    TE_UTILITY_EXPORT void te_frame_mark()
    {
        gFrameAllocator().MarkFrame();
    }

    TE_UTILITY_EXPORT void te_frame_clear()
    {
        gFrameAllocator().Clear();
    }
}
