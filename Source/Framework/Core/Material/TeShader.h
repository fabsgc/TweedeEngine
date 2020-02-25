#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Material/TeTechnique.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeSamplerState.h"

namespace te
{
    /** Shared memebers between SHADER_DATA_PARAM_DESC and SHADER_OBJECT_PARAM_DESC */
    struct SHADER_PARAM_COMMON
    {
        SHADER_PARAM_COMMON() = default;
        SHADER_PARAM_COMMON(String name, String gpuVariableName, String rendererSemantic = "")
            : Name(std::move(name))
            , GpuVariableName(gpuVariableName)
            , RendererSemantic(rendererSemantic)
        { }

        /** The name of the parameter. Name must be unique between all data and object parameters in a shader. */
        String Name;

        /** Name of the GPU variable in the GpuProgram that the parameter corresponds with. */
        String GpuVariableName;

        /**
         * Optional semantic that allows you to specify the use of this parameter in the renderer. The actual value of the
         * semantic depends on the current Renderer and its supported list of semantics. Elements with renderer semantics
         * should not be updated by the user, and will be updated by the renderer. These semantics will also be used to
         * determine if a shader is compatible with a specific renderer or not. Value of 0 signifies the parameter is not
         * used by the renderer.
         */
        String RendererSemantic;

        /** Index of the default value inside the Shader. Should not be set externally by the user. */
        UINT32 DefaultValueIdx = (UINT32)-1;

        /** Index to a set of optional attributes attached to the parameter. Should not be set externally by the user. */
        UINT32 AttributeIdx = (UINT32)-1;
    };

    /**
     * Describes a single data (int, Vector2, etc.) shader parameter.
     *
     * @see	Shader::addParameter().
     */
    struct SHADER_DATA_PARAM_DESC : SHADER_PARAM_COMMON
    {
        SHADER_DATA_PARAM_DESC() = default;
        SHADER_DATA_PARAM_DESC(String name, String gpuVariableName, GpuParamDataType type,
            String rendererSemantic = "", UINT32 arraySize = 1, UINT32 elementSize = 0)
            : SHADER_PARAM_COMMON(std::move(name), std::move(gpuVariableName), rendererSemantic)
            , Type(type)
            , ArraySize(arraySize)
            , ElementSize(elementSize)
        { }

        /** The type of the parameter, must be the same as the type in GpuProgram. */
        GpuParamDataType Type = GPDT_FLOAT1;

        /** If the parameter is an array, the number of elements in the array. Size of 1 means its not an array. */
        UINT32 ArraySize = 1;

        /**
         * Size of an individual element in the array, in bytes. You only need to set this if you are setting variable
         * length parameters, like structs. Otherwise the size is determined from the type.
         */
        UINT32 ElementSize = 0;
    };

    /**
     * Describes a single object (texture, sampler state, etc.) shader parameter.
     *
     * @see	Shader::AddParameter().
     */
    struct SHADER_OBJECT_PARAM_DESC : SHADER_PARAM_COMMON
    {
        SHADER_OBJECT_PARAM_DESC() = default;
        SHADER_OBJECT_PARAM_DESC(String name, String gpuVariableName, GpuParamObjectType type, String rendererSemantic = "")
            : SHADER_PARAM_COMMON(std::move(name), gpuVariableName, rendererSemantic)
            , Type(type)
        {
            GpuVariableNames.emplace_back(gpuVariableName);
        }

        /** The type of the parameter, must be the same as the type in GpuProgram. */
        GpuParamObjectType Type = GPOT_TEXTURE2D;

        /** Names of all GPU variables this shader parameter maps to. */
        Vector<String> GpuVariableNames;
    };

    /** Describes a shader parameter block. */
    struct SHADER_PARAM_BLOCK_DESC
    {
        String Name;
        bool Shared;
        String RendererSemantic;
        GpuBufferUsage Usage;
    };

    struct TE_CORE_EXPORT SHADER_DESC
    {
        SHADER_DESC();

        ~SHADER_DESC()
        { }

        /**
         * Registers a new data (int, Vector2, etc.) parameter you that you may then use via Material by providing the
         * parameter name. All parameters internally map to variables defined in GPU programs.
         *
         * @param[in]	paramDesc			Structure describing the parameter to add.
         * @param[in]	defaultValue		(optional) Pointer to the buffer containing the default value for this parameter
         *									(initial value that will be set when a material is initialized with this shader).
         *									The provided buffer must be of the correct size (depending on the element type
         *									and array size).
         *
         * @note	If multiple parameters are given with the same name but different types behavior is undefined.
         */
        void AddParameter(SHADER_DATA_PARAM_DESC paramDesc, UINT8* defaultValue = nullptr);

        /**
         * Registers a new object (texture, sampler state, etc.) parameter you that you may then use via Material by
         * providing the parameter name. All parameters internally map to variables defined in GPU programs. Multiple GPU
         * variables may be mapped to a single parameter in which case the first variable actually found in the program will
         * be used while others will be ignored.
         *
         * @param[in]	paramDesc			Structure describing the parameter to add.
         *
         * @note
         * If multiple parameters are given with the same name but different types behavior is undefined. You are allowed
         * to call this method multiple times in order to map multiple GPU variable names to a single parameter, but the
         * default value (if any) will only be recognized on the first call. Mapping multiple GPU variables to a single
         * parameter is useful when you are defining a shader that supports techniques across different render systems
         * where GPU variable names for the same parameters might differ.
         */
        void AddParameter(SHADER_OBJECT_PARAM_DESC paramDesc);

        /**
         * @see	SHADER_DESC::addParameter(SHADER_OBJECT_PARAM_DESC)
         *
         * @note
         * Specialized version of addParameter that accepts a default sampler value that will be used for initializing the
         * object parameter upon Material creation. Default sampler value is only valid if the object type is one of the
         * sampler types.
         */
        void AddParameter(SHADER_OBJECT_PARAM_DESC paramDesc, const SPtr<SamplerState>& defaultValue);

        /**
         * @see	SHADER_DESC::addParameter(SHADER_OBJECT_PARAM_DESC)
         *
         * @note
         * Specialized version of addParameter that accepts a default texture value that will be used for initializing the
         * object parameter upon Material creation. Default texture value is only valid if the object type is one of the
         * texture types.
         */
        void AddParameter(SHADER_OBJECT_PARAM_DESC paramDesc, const SPtr<Texture>& defaultValue);

        /**
         * Sorting type to use when performing sort in the render queue. Default value is sort front to back which causes
         * least overdraw and is preferable. Transparent objects need to be sorted back to front. You may also specify no
         * sorting and the elements will be rendered in the order they were added to the render queue.
         */
        QueueSortType QueueType = QueueSortType::FrontToBack;

        /**
         * Priority that allows you to control in what order are your shaders rendered. See QueuePriority for a list of
         * initial values. Shaders with higher priority will be rendered before shaders with lower priority, and
         * additionally render queue will only sort elements within the same priority group.
         *
         * @note
         * This is useful when you want all your opaque objects to be rendered before you start drawing your transparent
         * ones. Or to render your overlays after everything else. Values provided in QueuePriority are just for general
         * guidance and feel free to increase them or decrease them for finer tuning. (for example QueuePriority::Opaque +
         * 1).
         */
        INT32 QueuePriority = 0;

        /**
         * Enables or disables separable passes. When separable passes are disabled all shader passes will be executed in a
         * sequence one after another. If it is disabled the renderer is free to mix and match passes from different
         * objects to achieve best performance. (They will still be executed in sequence, but some other object may be
         * rendered in-between passes)
         *
         * @note	Shaders with transparency generally can't be separable, while opaque can.
         */
        bool SeparablePasses = false;

        /** Flags that let the renderer know how should it interpret the shader. */
        UINT32 Flags = 0;

        /** Techniques to initialize the shader with. */
        Vector<SPtr<Technique>> Techniques;

        Map<String, SHADER_DATA_PARAM_DESC> DataParams;
        Map<String, SHADER_OBJECT_PARAM_DESC> TextureParams;
        Map<String, SHADER_OBJECT_PARAM_DESC> BufferParams;
        Map<String, SHADER_OBJECT_PARAM_DESC> SamplerParams;
        Map<String, SHADER_PARAM_BLOCK_DESC> ParamBlocks;

        Vector<UINT8> DataDefaultValues;
        Vector<SPtr<SamplerState>> SamplerDefaultValues;
        Vector< SPtr<Texture>> TextureDefaultValues;

    private:
        /**
         * @copydoc	AddParameter(SHADER_OBJECT_PARAM_DESC)
         *
         * @note	Common method shared by different addParameter overloads.
         */
        void AddParameterInternal(SHADER_OBJECT_PARAM_DESC paramDesc, UINT32 defaultValueIdx);
    };

    class TE_CORE_EXPORT Shader : public Resource
    {
    public:
        virtual ~Shader();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** Returns the unique shader ID. */
        UINT32 GetId() const { return _id; }

        /** Returns the shader name. */
        const String& GetName() const { return _name; }

        /** Set shader name */
        void SetName(const String& name) { _name = name; }

        /**
         * Returns currently active queue sort type.
         *
         * @see		SHADER_DESC::QueueSortType
         */
        QueueSortType GetQueueSortType() const { return _desc.QueueType; }

        /**
         * Returns currently active queue priority.
         *
         * @see		SHADER_DESC::QueuePriority
         */
        INT32 GetQueuePriority() const { return _desc.QueuePriority; }

        /**
         * Returns if separable passes are allowed.
         *
         * @see		SHADER_DESC::SeparablePasses
         */
        bool GetAllowSeparablePasses() const { return _desc.SeparablePasses; }

        /**
         * Returns flags that control how the renderer interprets the shader. Actual interpretation of the flags depends on
         * the active renderer.
         */
        UINT32 GetFlags() const { return _desc.Flags; }

        /** Returns the total number of techniques in this shader. */
        UINT32 GetNumTechniques() const { return (UINT32)_desc.Techniques.size(); }

        /** Returns the list of all supported techniques based on current render API and renderer. */
        Vector<SPtr<Technique>> GetCompatibleTechniques() const;

        /** Returns a list of all techniques in this shader. */
        const Vector<SPtr<Technique>>& GetTechniques() const { return _desc.Techniques; }

        /**	Creates a new shader resource using the provided descriptor and techniques. */
        static HShader Create(const String& name, const SHADER_DESC& desc);

        /**	Returns a shader object but doesn't initialize it. */
        static SPtr<Shader> CreateEmpty();

        /**
         * Creates a new shader object using the provided descriptor and techniques.
         *
         * @note	Internal method. Use Create() for normal use.
         */
        static SPtr<Shader> _createPtr(const String& name, const SHADER_DESC& desc);

    public:
        /**	Checks is the provided object type a sampler. */
        static bool IsSampler(GpuParamObjectType type);

        /**	Checks is the provided object type a texture. */
        static bool IsTexture(GpuParamObjectType type);

        /**	Checks is the provided object type a load/store (unordered read/write) texture. */
        static bool IsLoadStoreTexture(GpuParamObjectType type);

        /** Checks is the provided object type a buffer. */
        static bool IsBuffer(GpuParamObjectType type);

        /**
         * Returns the size in bytes for a specific data type.
         *
         * @note	Returns 0 for variable size types like structures.
         */
        static UINT32 GetDataParamSize(GpuParamDataType type);

        /** Returns type of the parameter with the specified name. Throws exception if the parameter doesn't exist. */
        GpuParamType GetParamType(const String& name) const;

        /**
         * Returns description for a data parameter with the specified name. Throws exception if the parameter doesn't exist.
         */
        const SHADER_DATA_PARAM_DESC& GetDataParamDesc(const String& name) const;

        /**
         * Returns description for a texture parameter with the specified name. Throws exception if the parameter doesn't
         * exist.
         */
        const SHADER_OBJECT_PARAM_DESC& GetTextureParamDesc(const String& name) const;

        /**
         * Returns description for a sampler parameter with the specified name. Throws exception if the parameter doesn't
         * exist.
         */
        const SHADER_OBJECT_PARAM_DESC& GetSamplerParamDesc(const String& name) const;

        /**
         * Returns description for a buffer parameter with the specified name. Throws exception if the parameter doesn't
         * exist.
         */
        const SHADER_OBJECT_PARAM_DESC& GetBufferParamDesc(const String& name) const;

        /** Checks if the parameter with the specified name exists, and is a data parameter. */
        bool HasDataParam(const String& name) const;

        /**	Checks if the parameter with the specified name exists, and is a texture parameter. */
        bool HasTextureParam(const String& name) const;

        /** Checks if the parameter with the specified name exists, and is a sampler parameter. */
        bool HasSamplerParam(const String& name) const;

        /** Checks if the parameter with the specified name exists, and is a buffer parameter. */
        bool HasBufferParam(const String& name) const;

        /** Checks if the parameter block with the specified name exists. */
        bool HasParamBlock(const String& name) const;

        /**	Returns a map of all data parameters in the shader. */
        const Map<String, SHADER_DATA_PARAM_DESC>& GetDataParams() const { return _desc.DataParams; }

        /**	Returns a map of all texture parameters in the shader. */
        const Map<String, SHADER_OBJECT_PARAM_DESC>& GetTextureParams() const { return _desc.TextureParams; }

        /**	Returns a map of all buffer parameters in the shader. */
        const Map<String, SHADER_OBJECT_PARAM_DESC>& GetBufferParams() const { return _desc.BufferParams; }

        /** Returns a map of all sampler parameters in the shader. */
        const Map<String, SHADER_OBJECT_PARAM_DESC>& GetSamplerParams() const { return _desc.SamplerParams; }

        /** Returns a map of all parameter blocks. */
        const Map<String, SHADER_PARAM_BLOCK_DESC>& GetParamBlocks() const { return _desc.ParamBlocks; }

        /**
         * Returns a default texture for a parameter that has the specified default value index (retrieved from the
         * parameters descriptor).
         */
        SPtr<Texture> GetDefaultTexture(UINT32 index) const;

        /**
         * Returns a default sampler state for a parameter that has the specified default value index (retrieved from the
         * parameters descriptor).
         */
        SPtr<SamplerState> GetDefaultSampler(UINT32 index) const;

        /**
         * Returns a pointer to the internal buffer containing the default value for a data parameter that has the
         * specified default value index (retrieved from the parameters descriptor).
         */
        UINT8* GetDefaultValue(UINT32 index) const;

    protected:
        Shader(UINT32 id);
        Shader(const SHADER_DESC& desc, const String& name, UINT32 id);

    private:
        Shader();

    protected:
        SHADER_DESC _desc;
        String _name;
        UINT32 _id;

        static std::atomic<UINT32> NextShaderId;
    };
}
