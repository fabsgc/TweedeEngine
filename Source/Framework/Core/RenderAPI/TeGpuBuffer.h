#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "RenderAPI/TeHardwareBuffer.h"

namespace te
{
    /** Descriptor structure used for initialization of a GpuBuffer. */
    struct GPU_BUFFER_DESC
    {
        /** Number of elements in the buffer. */
        UINT32 ElementCount = 0;

        /**
         * Size of each individual element in the buffer, in bytes. Only needed if using non-standard buffer. If using
         * standard buffers element size is calculated from format and this must be zero.
         */
        UINT32 ElementSize = 0;

        /** Type of the buffer. Determines how is buffer seen by the GPU program and in what ways can it be used. */
        GpuBufferType Type = GBT_STANDARD;

        /** Format of the data in the buffer. Only relevant for standard buffers, must be BF_UNKNOWN otherwise. */
        GpuBufferFormat Format = BF_32X4F;

        /** Usage that tells the hardware how will be buffer be used. */
        GpuBufferUsage Usage = GBU_STATIC;
    };

    /** Information about a GpuBuffer. */
    class TE_CORE_EXPORT GpuBufferProperties
    {
    public:
        GpuBufferProperties(const GPU_BUFFER_DESC& desc);

        /**
         * Returns the type of the GPU buffer. Type determines which kind of views (if any) can be created for the buffer,
         * and how is data read or modified in it.
         */
        GpuBufferType GetType() const { return _desc.Type; }

        /** Returns format used by the buffer. Only relevant for standard buffers. */
        GpuBufferFormat GetFormat() const { return _desc.Format; }

        /** Returns buffer usage which determines how are planning on updating the buffer contents. */
        GpuBufferUsage GetUsage() const { return _desc.Usage; }

        /**	Returns number of elements in the buffer. */
        UINT32 GetElementCount() const { return _desc.ElementCount; }

        /**	Returns size of a single element in the buffer in bytes. */
        UINT32 GetElementSize() const { return _desc.ElementSize; }

    protected:
        friend class GpuBuffer;

        GPU_BUFFER_DESC _desc;
    };

    /**
     * Handles a generic GPU buffer that you may use for storing any kind of sequential data you wish to be accessible to
     * the GPU.
     */
    class TE_CORE_EXPORT GpuBuffer : public CoreObject, public HardwareBuffer
    {
    public:
        virtual ~GpuBuffer();

        /** Returns properties describing the buffer. */
        const GpuBufferProperties& GetProperties() const { return _properties; }

        /** @copydoc HardwareBuffer::ReadData */
        void ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::WriteData */
        void WriteData(UINT32 offset, UINT32 length, const void* source,
            BufferWriteType writeFlags = BWT_NORMAL, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::CopyData */
        void CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length,
            bool discardWholeBuffer = false) override;

        /** Returns the size of a single element in the buffer, of the provided format, in bytes. */
        static UINT32 GetFormatSize(GpuBufferFormat format);

        /**
         * Returns a view of this buffer with specified format/type.
         *
         * @param[in]	type			Type of buffer to view the contents as. Only supported values are GBT_STANDARD and
         *								GBT_STRUCTURED.
         * @param[in]	format			Format of the data in the buffer. Size of the underlying buffer must be divisible by
         *								the	size of an individual element of this format. Must be BF_UNKNOWN if buffer type
         *								is GBT_STRUCTURED.
         * @param[in]	elementSize		Size of the individual element in the buffer. Size of the underlying buffer must be
         *								divisible by this size. Must be 0 if buffer type is GBT_STANDARD (element size gets
         *								deduced from format).
         * @return						New view of the buffer, using the provided format and type.
         */
        SPtr<GpuBuffer> GetView(GpuBufferType type, GpuBufferFormat format, UINT32 elementSize = 0);

        /** @copydoc te::HardwareBufferManager::createGpuBuffer */
        static SPtr<GpuBuffer> Create(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * Creates a view of an existing hardware buffer. No internal buffer will be allocated and the provided buffer
         * will be used for all internal operations instead. Information provided in @p desc (such as element size and
         * count) must match the provided @p underlyingBuffer.
         */
        static SPtr<GpuBuffer> Create(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer);

    protected:
        friend class HardwareBufferManager;

        GpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);
        GpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer);

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc HardwareBuffer::map */
        void* Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::unmap */
        void Unmap() override;
    
    protected:
        GpuBufferProperties _properties;

        HardwareBuffer* _buffer = nullptr;
        SPtr<HardwareBuffer> _sharedBuffer;
        bool _isExternalBuffer = false;

        typedef void(*Deleter)(HardwareBuffer*);
        Deleter _bufferDeleter = nullptr;
    };
}
