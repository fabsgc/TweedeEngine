#pragma once

#include "TeCorePrerequisites.h"
#include "TeHardwareBuffer.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /** Descriptor structure used for initialization of an IndexBuffer. */
    struct INDEX_BUFFER_DESC
    {
        IndexType Type; /**< Index type, determines the size of a single index. */
        UINT32 NumIndices; /**< Number of indices can buffer can hold. */
        GpuBufferUsage Usage = GBU_STATIC; /**< Usage that tells the hardware how will be buffer be used. */
    };

    /** Contains information about an index buffer. */
    class TE_CORE_EXPORT IndexBufferProperties
    {
    public:
        IndexBufferProperties(IndexType idxType, UINT32 numIndexes);

        /** Returns the type of indices stored. */
        IndexType GetType() const { return _indexType; }

        /** Returns the number of indices this buffer can hold. */
        UINT32 GetNumIndices() const { return _numIndices; }

        /** Returns the size of a single index in bytes. */
        UINT32 GetIndexSize() const { return _indexSize; }

    protected:
        friend class IndexBuffer;

        IndexType _indexType;
        UINT32 _numIndices;
        UINT32 _indexSize;
    };

    class TE_CORE_EXPORT IndexBuffer : public CoreObject, public HardwareBuffer
    {
    public:
        IndexBuffer(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);
        virtual ~IndexBuffer();

        /** Returns information about the index buffer. */
        const IndexBufferProperties& GetProperties() const { return _properties; }

        /** @copydoc HardwareBuffer::ReadData */
        void ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::WriteData */
        void WriteData(UINT32 offset, UINT32 length, const void* source,
            BufferWriteType writeFlags = BWT_NORMAL, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::CopyData */
        void CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length,
            bool discardWholeBuffer = false) override;

        /** @copydoc HardwareBufferManager::CreateIndexBuffer */
        static SPtr<IndexBuffer> Create(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

    protected:
        friend class HardwareBufferManager;

        /** @copydoc HardwareBuffer::Map */
        void* Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx) override;

        /** @copydoc HardwareBuffer::Unmap */
        void Unmap() override;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

    protected:
        IndexBufferProperties _properties;
        GpuBufferUsage _usage;

        HardwareBuffer* _buffer = nullptr;
        SPtr<HardwareBuffer> _sharedBuffer;

        typedef void(*Deleter)(HardwareBuffer*);
        Deleter _bufferDeleter = nullptr;
    };
}
