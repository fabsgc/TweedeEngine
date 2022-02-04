#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TePoolAllocator.h"

namespace te
{
#if TE_PROFILING_ENABLED
    #define TE_INC_PROFILER_GPU(Stat) gProfilerGPU().Inc##Stat()
    #define TE_ADD_PROFILER_GPU(Stat, Count) gProfilerGPU().Add##Stat(Count)
#else
    #define TE_INC_PROFILER_GPU(Stat)
    #define TE_ADD_PROFILER_GPU(Stat, Count)
#endif

    /** Contains various profiler statistics about a single GPU sample. */
    struct GPUSample
    {
        UINT64 Time = 0; /**< Time in microseconds it took to execute the sampled block. */

        UINT32 NumDrawCalls = 0; /**< Number of draw calls that happened. */
        UINT32 NumComputeCalls = 0; /**< Number of compute calls that happened. */
        UINT32 NumRenderTargetChanges = 0; /**< How many times was render target changed. */
        UINT32 NumPresents = 0; /**< How many times did a buffer swap happen on a double buffered render target. */
        UINT32 NumClears = 0; /**< How many times was render target cleared. */

        UINT32 NumVertices = 0; /**< Total number of vertices sent to the GPU. */
        UINT32 NumPrimitives = 0; /**< Total number of primitives sent to the GPU. */
        UINT32 NumInstances = 0; /**< Total number of instances sent to the GPU. */

        UINT32 NumPipelineStateChanges = 0; /**< How many times did the pipeline state change. */

        UINT32 NumGpuParamBinds = 0; /**< How many times were GPU parameters bound. */
        UINT32 NumVertexBufferBinds = 0; /**< How many times was a vertex buffer bound. */
        UINT32 NumIndexBufferBinds = 0; /**< How many times was an index buffer bound. */

        UINT32 NumResourceWrites = 0; /**< How many times were GPU resources written to. */
        UINT32 NumResourceReads = 0; /**< How many times were GPU resources read from. */

        UINT32 NumObjectsCreated = 0; /**< How many GPU objects were created. */
        UINT32 NumObjectsDestroyed = 0; /**< How many GPU objects were destroyed. */

        UINT64 GPUMemory = 0; /**< How many GPU memory is available in byte. */
        UINT64 SharedMemory = 0; /**< How many shared memory is available in byte.  */
        UINT64 UsedGPUMemory = 0; /**< How many GPU memory is used in byte. */
        
    };

    /**
     * Profiler that measures time and amount of various GPU operations.
     */
    class TE_CORE_EXPORT ProfilerGPU : public Module<ProfilerGPU>
    {
    public:
        ProfilerGPU();
        virtual ~ProfilerGPU() = default;

        /**
         * Signals a start of a new frame. This call must be followed
         * by endFrame(), and any sampling operations must happen between BeginFrame() and EndFrame().
         */
        void BeginFrame();

        /** Signals an end of the currently sampled frame.*/
        void EndFrame();

        /** Enable profiler */
        void Enable(bool enable);

        /* @copydoc ProfilerGPU::Enable */
        bool IsEnabled() { return _enabled; }

        /* Returns last metrics sample */
        const GPUSample& GetSample() const { return _sample; }

        /** Increments draw call counter indicating how many times were render system API Draw methods called. */
        void IncNumDrawCalls();

        /** Increments compute call counter indicating how many times were compute shaders dispatched. */
        void IncNumComputeCalls();

        /** Increments render target change counter indicating how many times did the active render target change. */
        void IncNumRenderTargetChanges();

        /** Increments render target present counter indicating how many times did the buffer swap happen. */
        void IncNumPresents();

        /**
         * Increments render target clear counter indicating how many times did the target the cleared, entirely or
         * partially.
         */
        void IncNumClears();

        /** Increments vertex draw counter indicating how many vertices were sent to the pipeline. */
        void AddNumVertices(UINT32 count);

        /** Increments primitive draw counter indicating how many primitives were sent to the pipeline. */
        void AddNumPrimitives(UINT32 count);

        /** Increments instances draw counter indicating how many instanced meshes were sent to the pipeline. */
        void AddNumInstances(UINT32 count);

        /** Increments pipeline state change counter indicating how many times was a pipeline state bound. */
        void IncNumPipelineStateChanges();

        /** Increments GPU parameter change counter indicating how many times were GPU parameters bound to the pipeline. */
        void IncNumGpuParamBinds();

        /** Increments vertex buffer change counter indicating how many times was a vertex buffer bound to the pipeline. */
        void IncNumVertexBufferBinds();

        /** Increments index buffer change counter indicating how many times was a index buffer bound to the pipeline. */
        void IncNumIndexBufferBinds();

        /** Increments created GPU resource counter. */
        void IncResCreated();

        /** Increments destroyed GPU resource counter. */
        void IncResDestroyed();

        /** Increments GPU resource read counter. */
        void IncResRead();

        /** Increments GPU resource write counter. */
        void IncResWrite();

    private:
        /*
         * Reset all metric from previous frame 
         */
        void Reset();

    private:
        GPUSample _sample;
        bool _enabled;
        bool _frameBegan;
    };

    /** Provides global access to ProfilerGPU instance. */
    TE_CORE_EXPORT ProfilerGPU& gProfilerGPU();

    /** Profiling macros that allow profiling functionality to be disabled at compile time. */
#if TE_PROFILING_ENABLED
#   define TE_GPU_PROFILE_BEGIN() gProfilerGPU().BeginFrame();
#   define TE_GPU_PROFILE_END() gProfilerGPU().EndFrame();
#else
#   define TE_GPU_PROFILE_BEGIN()
#   define TE_GPU_PROFILE_END()
#endif
}
