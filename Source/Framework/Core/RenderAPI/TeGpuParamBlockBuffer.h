#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /**
	 * Represents a GPU parameter block buffer. Parameter block buffers are bound to GPU programs which then fetch
	 * parameters from those buffers.
	 *
	 * Writing or reading from this buffer will translate directly to API calls that update the GPU.
	 */
	class TE_CORE_EXPORT GpuParamBlockBuffer : public CoreObject
	{
    public:
		GpuParamBlockBuffer(UINT32 size, GpuBufferUsage usage, GpuDeviceFlags deviceMask);
		virtual ~GpuParamBlockBuffer();

        /**
		 * Write some data to the specified offset in the buffer.
		 *
		 * @note	All values are in bytes. Actual hardware buffer update is delayed until rendering.
		 */
		void Write(UINT32 offset, const void* data, UINT32 size);

		/**
		 * Read some data from the specified offset in the buffer.
		 *			
		 * @note	All values are in bytes. This reads from the cached CPU buffer and not from the GPU.
		 */
		void Read(UINT32 offset, void* data, UINT32 size);

        /**
		 * Writes all of the specified data to the buffer. Data size must be the same size as the buffer.
		 *
		 * @param[in]	data		Data to write. Must match the size of the buffer.
		 * @param[in]	queueIdx	Device queue to perform the write operation on. See @ref queuesDoc.
		 */
		void WriteToGPU(const UINT8* data, UINT32 queueIdx = 0);

		/**
		 * Flushes any cached data into the actual GPU buffer.
		 *
		 * @param[in]	queueIdx	Device queue to perform the write operation on. See @ref queuesDoc.
		 */
		void FlushToGPU(UINT32 queueIdx = 0);

		/**
		 * Clear specified section of the buffer to zero.
		 *
		 * @note	All values are in bytes. Actual hardware buffer update is delayed until rendering.
		 */
		void ZeroOut(UINT32 offset, UINT32 size);

		/** Returns internal cached data of the buffer. */
		const UINT8* GetCachedData() const { return _cachedData; }

		/**	Returns the size of the buffer in bytes. */
		UINT32 GetSize() const { return _size; }

		/** @copydoc HardwareBufferManager::CreateGpuParamBlockBuffer */
		static SPtr<GpuParamBlockBuffer> Create(UINT32 size, GpuBufferUsage usage = GBU_DYNAMIC,
			GpuDeviceFlags deviceMask = GDF_DEFAULT);

    protected:
        friend class HardwareBufferManager;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

    protected:        
        HardwareBuffer* _buffer;
        GpuBufferUsage _usage;
		UINT32 _size;
		UINT8* _cachedData;
        bool _GPUBufferDirty = false;
    };
}
