#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeGpuParam.h"
#include "CoreUtility/TeCoreObject.h"
#include "Math/TeMatrixNxM.h"
#include "TeGpuPipelineParamInfo.h"
#include "TeGpuBuffer.h"

namespace te
{
    class TE_CORE_EXPORT GpuParams : public CoreObject
    {
    public:
        /** Surface that covers all texture sub-resources. */
        static const TextureSurface COMPLETE;

        virtual ~GpuParams();

        bool HasChanged() const { return _hasChanged; }

        /** Returns a description of all stored parameters. */
        SPtr<GpuParamDesc> GetParamDesc(GpuProgramType type) const { return _paramInfo->GetParamDesc(type); }

        /** Gets the object that contains the processed information about all parameters. */
        SPtr<GpuPipelineParamInfo> GetParamInfo() const { return _paramInfo; }

        /** Returns the size of a data parameter with the specified name, in bytes. Returns 0 if such parameter doesn't exist. */
        UINT32 GetDataParamSize(GpuProgramType type, const String& name) const;

        /** Checks if parameter with the specified name exists. */
        bool HasParam(GpuProgramType type, const String& name) const;

        /**	Checks if texture parameter with the specified name exists. */
        bool HasTexture(GpuProgramType type, const String& name) const;

        /**	Checks if buffer parameter with the specified name exists. */
        bool HasBuffer(GpuProgramType type, const String& name) const;

        /**	Checks if sampler state parameter with the specified name exists. */
        bool HasSamplerState(GpuProgramType type, const String& name) const;

        /** Checks if a parameter block with the specified name exists. */
        bool HasParamBlock(GpuProgramType type, const String& name) const;

        /**	Gets a descriptor for a parameter block buffer with the specified name. */
        GpuParamBlockDesc* GetParamBlockDesc(GpuProgramType type, const String& name) const;

        /**	Gets a parameter block buffer from the specified set/slot combination. */
        SPtr<GpuParamBlockBuffer> GetParamBlockBuffer(UINT32 set, UINT32 slot) const;

        /**	Gets a texture bound to the specified set/slot combination. */
        SPtr<Texture> GetTexture(UINT32 set, UINT32 slot) const;

        /**	Gets a buffer bound to the specified set/slot combination. */
        SPtr<GpuBuffer> GetBuffer(UINT32 set, UINT32 slot) const;

        /**	Gets a sampler state bound to the specified set/slot combination. */
        SPtr<SamplerState> GetSamplerState(UINT32 set, UINT32 slot) const;

        /** Gets information that determines which texture surfaces to bind as a sampled texture parameter. */
        const TextureSurface& GetTextureSurface(UINT32 set, UINT32 slot) const;

        /**
         * Assigns the provided parameter block buffer to a buffer with the specified name, for the specified GPU program
         * stage. Any following parameter reads or writes that are referencing that buffer will use the new buffer.
         *
         * It is up to the caller to guarantee the provided buffer matches parameter block descriptor for this slot.
         */
        void SetParamBlockBuffer(GpuProgramType type, const String& name, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer);

        /**
         * Assigns the provided parameter block buffer to a buffer with the specified name, for any stages that reference
         * the buffer. Any following parameter reads or writes that are referencing that buffer will use the new buffer.
         *
         * It is up to the caller to guarantee the provided buffer matches parameter block descriptor for this slot.
         * It is up to the caller that all stages using this buffer name refer to the same buffer type.
         */
        void SetParamBlockBuffer(const String& name, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer);

        /**
         * Sets the parameter buffer with the specified set/slot combination.Any following parameter reads or writes that are
         * referencing that buffer will use the new buffer. Set/slot information for a specific buffer can be extracted
         * from GPUProgram's GpuParamDesc structure.
         *
         * It is up to the caller to guarantee the provided buffer matches parameter block descriptor for this slot.
         */
        virtual void SetParamBlockBuffer(UINT32 set, UINT32 slot, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer);

        /**	Sets a texture at the specified set/slot combination. */
        virtual void SetTexture(UINT32 set, UINT32 slot, const SPtr<Texture>& texture,
            const TextureSurface& surface = COMPLETE);

        /**	Sets a buffer at the specified set/slot combination. */
        virtual void SetBuffer(UINT32 set, UINT32 slot, const SPtr<GpuBuffer>& buffer);

        /**	Sets a sampler state at the specified set/slot combination. */
        virtual void SetSamplerState(UINT32 set, UINT32 slot, const SPtr<SamplerState>& sampler);

        /**
         * @copydoc te::GpuParams::Create(const SPtr<GraphicsPipelineState>&, GpuDeviceFlags)
         * @param[in]	deviceMask		Mask that determines on which GPU devices should the buffer be created on.
         */
        static SPtr<GpuParams> Create(const SPtr<GraphicsPipelineState>& pipelineState,
            GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * @copydoc te::GpuParams::Create(const SPtr<GpuPipelineParamInfo>&, GpuDeviceFlags)
         * @param[in]	deviceMask		Mask that determines on which GPU devices should the buffer be created on.
         */
        static SPtr<GpuParams> Create(const SPtr<GpuPipelineParamInfo>& paramInfo,
            GpuDeviceFlags deviceMask = GDF_DEFAULT);

    protected:
        friend class HardwareBufferManager;

        GpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask);

        /**	Gets a descriptor for a data parameter with the specified name. */
        GpuParamDataDesc* GetParamDesc(GpuProgramType type, const String& name) const;

        /** @copydoc CoreObject::GetThisPtr */
        SPtr<GpuParams> _getThisPtr() const;

    protected:
        SPtr<GpuPipelineParamInfo> _paramInfo;

        /** Data for a single bound texture. */
        struct TextureData
        {
            SPtr<Texture> Tex;
            TextureSurface Surface;
        };

        SPtr<GpuParamBlockBuffer>* _paramBlockBuffers = nullptr;
        TextureData* _sampledTextureData = nullptr;
        SPtr<GpuBuffer>* _buffers = nullptr;
        SPtr<SamplerState>* _samplerStates = nullptr;

        bool _hasChanged = false;
    };
}
