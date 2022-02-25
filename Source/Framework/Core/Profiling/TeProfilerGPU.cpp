#include "TeProfilerGPU.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Utility/TeTime.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(ProfilerGPU)

    ProfilerGPU::ProfilerGPU()
        : _sample(GPUSample())
        , _enabled(true)
        , _frameBegan(false)
    { }

    void ProfilerGPU::BeginFrame()
    { 
        if (!_enabled)
        {
            _frameBegan = false;
            return;
        }

        Reset();
        _frameBegan = true;
        _sample.Time = gTime().GetTimePrecise();
    }

    void ProfilerGPU::EndFrame()
    { 
        if (_frameBegan)
        {
            _frameBegan = false;
            _sample.Time = (gTime().GetTimePrecise() - _sample.Time);

            _sample.GPUMemory = RenderAPI::Instance().GetGPUMemory();
            _sample.SharedMemory = RenderAPI::Instance().GetSharedMemory();
            _sample.UsedGPUMemory = RenderAPI::Instance().GetUsedGPUMemory();
        }
    }

    void ProfilerGPU::Enable(bool enable)
    {
        _enabled = enable;

        if (!_enabled)
            Reset();
    }

    void ProfilerGPU::Reset()
    {
        _frameBegan = false;

        _sample.Time = 0;

        _sample.NumDrawCalls = 0;
        _sample.NumComputeCalls = 0;
        _sample.NumRenderTargetChanges = 0;
        _sample.NumPresents = 0;
        _sample.NumClears = 0;

        _sample.NumVertices = 0;
        _sample.NumPrimitives = 0;
        _sample.NumInstances = 0;

        _sample.NumPipelineStateChanges = 0;

        _sample.NumGpuParamBinds = 0;
        _sample.NumVertexBufferBinds = 0;
        _sample.NumIndexBufferBinds = 0;

        _sample.NumResourceWrites = 0;
        _sample.NumResourceReads = 0;

        _sample.NumObjectsCreated = 0;
        _sample.NumObjectsDestroyed = 0;

        _sample.GPUMemory = 0;
        _sample.SharedMemory = 0;
        _sample.UsedGPUMemory = 0;
    }

    void ProfilerGPU::IncNumDrawCalls() 
    { 
        if (!_frameBegan)
            return;

        _sample.NumDrawCalls++; 
    }

    void ProfilerGPU::IncNumComputeCalls() 
    { 
        if (!_frameBegan)
            return;

        _sample.NumComputeCalls++; 
    }

    void ProfilerGPU::IncNumRenderTargetChanges() 
    { 
        if (!_frameBegan)
            return;

        _sample.NumRenderTargetChanges++; 
    }

    void ProfilerGPU::IncNumPresents() 
    { 
        if (!_frameBegan)
            return;

        _sample.NumPresents++; 
    }

    void ProfilerGPU::IncNumClears() 
    {
        if (!_frameBegan)
            return;

        _sample.NumClears++; 
    }

    void ProfilerGPU::AddNumVertices(UINT32 count) 
    { 
        if (!_frameBegan)
            return;

        _sample.NumVertices += count; 
    }

    void ProfilerGPU::AddNumPrimitives(UINT32 count) 
    { 
        if (!_frameBegan)
            return;

        _sample.NumPrimitives += count; 
    }

    void ProfilerGPU::AddNumInstances(UINT32 count)
    {
        if (!_frameBegan)
            return;

        _sample.NumInstances += count;
    }

    void ProfilerGPU::IncNumPipelineStateChanges() 
    { 
        if (!_frameBegan)
            return;

        _sample.NumPipelineStateChanges++; 
    }

    void ProfilerGPU::IncNumGpuParamBinds()
    {
        if (!_frameBegan)
            return;

        _sample.NumGpuParamBinds++; 
    }

    void ProfilerGPU::IncNumVertexBufferBinds() 
    { 
        if (!_frameBegan)
            return;

        _sample.NumVertexBufferBinds++; 
    }

    void ProfilerGPU::IncNumIndexBufferBinds() 
    {
        if (!_frameBegan)
            return;

        _sample.NumIndexBufferBinds++; 
    }

    void ProfilerGPU::IncResCreated()
    {
        if (!_frameBegan)
            return;

        _sample.NumObjectsCreated++;
    }

    void ProfilerGPU::IncResDestroyed()
    {
        if (!_frameBegan)
            return;

        _sample.NumObjectsDestroyed++;
    }

    void ProfilerGPU::IncResRead()
    {
        if (!_frameBegan)
            return;

        _sample.NumResourceReads++;
    }

    void ProfilerGPU::IncResWrite()
    {
        if (!_frameBegan)
            return;

        _sample.NumResourceWrites++;
    }

    ProfilerGPU& gProfilerGPU()
    {
        return ProfilerGPU::Instance();
    }
}
