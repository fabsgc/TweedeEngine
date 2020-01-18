#pragma once

#include "TeCorePrerequisites.h"
#include "TeHardwareBuffer.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /** Descriptor structure used for initialization of a VertexBuffer. */
    struct VERTEX_BUFFER_DESC
    {
        UINT32 VertexSize; /**< Size of a single vertex in the buffer, in bytes. */
        UINT32 NumVerts; /**< Number of vertices the buffer can hold. */
        GpuBufferUsage Usage = GBU_STATIC; /**< Usage that tells the hardware how will be buffer be used. */
        bool StreamOut = false; /**< If true the buffer will be usable for streaming out data from the GPU. */
    };

    /** Contains information about a vertex buffer buffer. */
	class TE_CORE_EXPORT VertexBufferProperties
	{
	public:
		VertexBufferProperties(UINT32 numVertices, UINT32 vertexSize);

		/**	Gets the size in bytes of a single vertex in this buffer. */
		UINT32 GetVertexSize() const { return _vertexSize; }

		/**	Get the number of vertices in this buffer. */
		UINT32 GetNumVertices() const { return _numVertices; }

	protected:
		friend class VertexBuffer;

		UINT32 _numVertices;
		UINT32 _vertexSize;
	};

    /** Specific implementation of a VertexBuffer. */
    class TE_CORE_EXPORT VertexBuffer : public CoreObject, public HardwareBuffer
    {
    public:
		VertexBuffer(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);
		virtual ~VertexBuffer();

        /**	Returns information about the vertex buffer. */
		const VertexBufferProperties& GetProperties() const { return _properties; }

		/** @copydoc HardwareBuffer::ReadData */
		void ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

		/** @copydoc HardwareBuffer::WriteData */
		void WriteData(UINT32 offset, UINT32 length, const void* source,
			BufferWriteType writeFlags = BWT_NORMAL, UINT32 queueIdx = 0) override;

		/** @copydoc HardwareBuffer::CopyData */
		void CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length,
			bool discardWholeBuffer = false) override;

        /** @copydoc HardwareBufferManager::CreateVertexBuffer */
        static SPtr<VertexBuffer> Create(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /** You can't have more that 8 VertexBufer binded at the same time */
        static const int MAX_SEMANTIC_IDX = 8;

    protected:
		friend class HardwareBufferManager;

		/** @copydoc HardwareBuffer::Map */
		void* Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx) override;

		/** @copydoc HardwareBuffer::Unmap */
		void Unmap() override;

		/** @copydoc CoreObject::Initialize */
		void Initialize() override;

    protected:
		VertexBufferProperties _properties;
        GpuBufferUsage _usage;
		bool _streamOut;

		HardwareBuffer* _buffer = nullptr;
		SPtr<HardwareBuffer> _sharedBuffer;

		typedef void(*Deleter)(HardwareBuffer*);
		Deleter _bufferDeleter = nullptr;

        // TODO
    };
}
