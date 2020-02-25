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

        /**
        * Returns a view of this buffer that can be used for load-store operations. Buffer must have been created with
        * the GBU_LOADSTORE usage flag.
        *
        * @param[in]	type			Type of buffer to view the contents as. Only supported values are GBT_STANDARD and
        *								GBT_STRUCTURED.
        * @param[in]	format			Format of the data in the buffer. Size of the underlying buffer must be divisible by
        *								the	size of an individual element of this format. Must be BF_UNKNOWN if buffer type
        *								is GBT_STRUCTURED.
        * @param[in]	elementSize		Size of the individual element in the buffer. Size of the underlying buffer must be
        *								divisible by this size. Must be 0 if buffer type is GBT_STANDARD (element size gets
        *								deduced from format).
        * @return						Buffer usable for load store operations or null if the operation fails. Failure
        *								can happen if the buffer hasn't been created with GBU_LOADSTORE usage or if the
        *								element size doesn't divide the current buffer size.
        */
        SPtr<GpuBuffer> GetLoadStore(GpuBufferType type, GpuBufferFormat format, UINT32 elementSize = 0);

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
        Vector<SPtr<GpuBuffer>> _loadStoreViews;

        typedef void(*Deleter)(HardwareBuffer*);
        Deleter _bufferDeleter = nullptr;
    };
}
