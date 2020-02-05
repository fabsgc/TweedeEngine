#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeGpuParam.h"
#include "CoreUtility/TeCoreObject.h"
#include "Math/TeMatrixNxM.h"
#include "TeGpuPipelineParamInfo.h"
#include "TeGpuBuffer.h"

namespace te
{
    /**	Helper structure whose specializations convert an engine data type into a GPU program data parameter type.  */
    template<class T> struct TGpuDataParamInfo { enum { TypeId = GPDT_STRUCT }; };
    template<> struct TGpuDataParamInfo < float > { enum { TypeId = GPDT_FLOAT1 }; };
    template<> struct TGpuDataParamInfo < Vector2 > { enum { TypeId = GPDT_FLOAT2 }; };
    template<> struct TGpuDataParamInfo < Vector3 > { enum { TypeId = GPDT_FLOAT3 }; };
    template<> struct TGpuDataParamInfo < Vector4 > { enum { TypeId = GPDT_FLOAT4 }; };
    template<> struct TGpuDataParamInfo < int > { enum { TypeId = GPDT_INT1 }; };
    template<> struct TGpuDataParamInfo < Vector2I > { enum { TypeId = GPDT_INT2 }; };
    template<> struct TGpuDataParamInfo < Vector3I > { enum { TypeId = GPDT_INT3 }; };
    template<> struct TGpuDataParamInfo < Vector4I > { enum { TypeId = GPDT_INT4 }; };
    template<> struct TGpuDataParamInfo < Matrix2 > { enum { TypeId = GPDT_MATRIX_2X2 }; };
    template<> struct TGpuDataParamInfo < Matrix2x3 > { enum { TypeId = GPDT_MATRIX_2X3 }; };
    template<> struct TGpuDataParamInfo < Matrix2x4 > { enum { TypeId = GPDT_MATRIX_2X4 }; };
    template<> struct TGpuDataParamInfo < Matrix3 > { enum { TypeId = GPDT_MATRIX_3X3 }; };
    template<> struct TGpuDataParamInfo < Matrix3x2 > { enum { TypeId = GPDT_MATRIX_3X2 }; };
    template<> struct TGpuDataParamInfo < Matrix3x4 > { enum { TypeId = GPDT_MATRIX_3X4 }; };
    template<> struct TGpuDataParamInfo < Matrix4 > { enum { TypeId = GPDT_MATRIX_4X4 }; };
    template<> struct TGpuDataParamInfo < Matrix4x2 > { enum { TypeId = GPDT_MATRIX_4X2 }; };
    template<> struct TGpuDataParamInfo < Matrix4x3 > { enum { TypeId = GPDT_MATRIX_4X3 }; };
    template<> struct TGpuDataParamInfo < Color > { enum { TypeId = GPDT_COLOR }; };

    class TE_CORE_EXPORT GpuParams : public CoreObject
    {
    public:
        /** Surface that covers all texture sub-resources. */
        static const TextureSurface COMPLETE;

        virtual ~GpuParams();

        /** Allow to know if a parameter has changed */
        bool HasChanged() const { return _hasChanged; }

        /** Set _hasChanged attribute */
        void SetChanged(bool changed = true) { _hasChanged = changed; }

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

        /**
         * Assigns the provided texture to a buffer with the specified name, for the specified GPU program
         * It is up to the caller to guarantee the provided buffer matches parameter block descriptor for this slot.
         */
        void SetTexture(GpuProgramType type, const String& name, const SPtr<Texture>& texture, const TextureSurface& surface = COMPLETE);

        /**
         * Assigns the provided texture buffer to a buffer with the specified name, for any stages that reference
         * the buffer. Any following parameter reads or writes that are referencing that buffer will use the new buffer.
         *
         * It is up to the caller to guarantee the provided buffer matches parameter block descriptor for this slot.
         * It is up to the caller that all stages using this buffer name refer to the same buffer type.
         */
        void SetTexture(const String& name, const SPtr<Texture>& texture, const TextureSurface& surface = COMPLETE);

        /**	Sets a texture at the specified set/slot combination. */
        virtual void SetTexture(UINT32 set, UINT32 slot, const SPtr<Texture>& texture, const TextureSurface& surface = COMPLETE);

        /**
         * Assigns the provided gpu buffer to a buffer with the specified name, for the specified GPU program
         * It is up to the caller to guarantee the provided gpu buffer matches parameter block descriptor for this slot.
         */
        void SetBuffer(GpuProgramType type, const String& name, const SPtr<GpuBuffer>& buffer);

        /**
         * Assigns the provided gpu buffer to a buffer with the specified name, for any stages that reference
         * the buffer. Any following parameter reads or writes that are referencing that buffer will use the new buffer.
         *
         * It is up to the caller to guarantee the provided buffer matches parameter block descriptor for this slot.
         * It is up to the caller that all stages using this buffer name refer to the same buffer type.
         */
        void SetBuffer(const String& name, const SPtr<GpuBuffer>& buffer);

        /**	Sets a buffer at the specified set/slot combination. */
        virtual void SetBuffer(UINT32 set, UINT32 slot, const SPtr<GpuBuffer>& buffer);

        /**
         * Assigns the provided gpu buffer to a buffer with the specified name, for the specified GPU program
         * It is up to the caller to guarantee the provided gpu buffer matches parameter block descriptor for this slot.
         */
        void SetSamplerState(GpuProgramType type, const String& name, const SPtr<SamplerState>& sampler);

        /**
         * Assigns the provided sampler state to a buffer with the specified name, for any stages that reference
         * the buffer. Any following parameter reads or writes that are referencing that buffer will use the new buffer.
         *
         * It is up to the caller to guarantee the provided buffer matches parameter block descriptor for this slot.
         * It is up to the caller that all stages using this buffer name refer to the same buffer type.
         */
        void SetSamplerState(const String& name, const SPtr<SamplerState>& sampler);

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

        /** Contains a lookup table for sizes of all data parameters. Sizes are in bytes. */
        const static GpuDataParamInfos PARAM_SIZES;

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
