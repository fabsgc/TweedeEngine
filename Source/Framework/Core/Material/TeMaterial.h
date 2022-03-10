#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeGpuBuffer.h"
#include "Math/TeVector2.h"
#include "Image/TeColor.h"
#include "TeShaderVariation.h"

#include <atomic>

namespace te
{
    /** Flags that signal in what way did the Material change. */
    enum class MaterialDirtyFlags
    {
        /** Material parameter changed. */
        Param				= 1 << 0,
        /** Dependant resource has been loaded and/or changed. (e.g. a texture assigned to a parameter. */
        ParamResource		= 1 << 1,
        /** Material shader has changed. */
        Shader				= 2 << 2
    };

    /** Structure used when searching for a specific technique in a Material. */
    struct FIND_TECHNIQUE_DESC
    {
        static constexpr UINT32 MAX_NUM_TAGS = 10;

        /** A set of tags that the technique must have. */
        StringID Tags[MAX_NUM_TAGS];

        /** Number of valid tags in the @p tags array. */
        UINT32 NumTags = 0;

        /** Specified variation of the technique. Parameters not specified in the variation are assumed to be irrelevant. */
        const ShaderVariation* Variation = nullptr;

        /**
         * Determines should the parameters in @p variation override any parameters that might have been defined on the
         * Material itself. If false then you are guaranteed to search only over the subset of techniques that match the
         * Material's internal variaton parameters. If true then you can search outside that range by setting a variation
         * parameter to some different value. Overriding can be useful for renderers which might need to override the user's
         * choice of variation.
         */
        bool Override = false;

        /** Registers a new tag to look for when searching for the technique. */
        void AddTag(const StringID& tag)
        {
            TE_ASSERT_ERROR_SHORT(NumTags < MAX_NUM_TAGS);

            Tags[NumTags] = tag;
            NumTags++;
        }
    };

    struct MaterialProperties
    {
        Color BaseColor        = Color(0.6f, 0.6f, 0.6f);

        float Roughness        = 0.5f;
        float Metallic         = 0.5f;
        float Reflectance      = 0.0f;
        float AmbientOcclusion = 0.0f;

        Color Emissive         = Color::Black;

        /**
         * Specify if this material must compute IBL using textures
         * coming from this material, skybox or light probe
         * Priority : Material > Light Probe > Skybox
        */
        bool UseIBL = true;

        /**
         * Does this material must compute diffuse irradiance using 
         * a texture stored on this material
         */
        bool UseDiffuseIrrMap = false;
    };

    struct MaterialTextures
    {
        String DiffuseMap;
        String EmissiveMap;
        String NormalMap;
        String SpecularMap;
        String BumpMap;
        String ParallaxMap;
        String TransparencyMap;
        String ReflectionMap;
        String OcclusionMap;
        String EnvironmentMap;
        String IrradianceMap;
    };

    /**
     * Material that controls how objects are rendered. It is represented by a shader and parameters used to set up that
     * shader. It provides a simple interface for manipulating the parameters.
     */
    class TE_CORE_EXPORT Material : public Resource
    {
    public:
        virtual ~Material();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TypeID_Core::TID_Material; }

        /** Returns the unique shader ID. */
        UINT32 GetId() const { return _id; }

        /**
         * Sets a shader that will be used by the material. Material will be initialized using all compatible techniques
         * from the shader. Shader must be set before doing any other operations with the material.
         */
        void SetShader(const SPtr<Shader>& shader);

        /** @copydoc SetShader */
        void SetShader(const HShader& shader);

        /**
         * Set of parameters that determine which subset of techniques in the assigned shader should be used. Only the
         * techniques that have the provided parameters with the provided values will match. This will control which
         * technique is considered the default technique and which subset of techniques are searched during a call to
         * findTechnique().
         */
        void SetVariation(const ShaderVariation& variation);

        /** Returns the currently active shader. */
        SPtr<Shader> GetShader() const { return _shader; } 

        /**
         * Set of parameters that determine which subset of techniques in the assigned shader should be used. Only the
         * techniques that have the provided parameters with the provided values will match. This will control which
         * technique is considered the default technique and which subset of techniques are searched during a call to
         * findTechnique().
         */
        const ShaderVariation& GetVariation() const { return _variation; }

        /** Returns the total number of techniques supported by this material. */
        UINT32 GetNumTechniques() const;

        /** Returns the technique at the specified index. */
        const SPtr<Technique>& GetTechnique(UINT32 idx) const;

        /**
         * Attempts to find a technique matching the specified variation and tags among the supported techniques.
         *
         * @param[in]	desc				Object containing an optional set of tags and a set of variation parameters to
         *									look for.
         * @return							First technique that matches the tags & variation parameters specified in
         *									@p desc.
         */
        UINT32 FindTechnique(const FIND_TECHNIQUE_DESC& desc) const;

        /**
         * Finds the index of the default (primary) technique to use. This will be the first technique that matches the
         * currently set variation parameters (if any).
         */
        UINT32 GetDefaultTechnique() const;

        /**
         * Returns the number of passes that are used by the technique at the specified index.
         *
         * @param[in]	techniqueIdx	Index of the technique to retrieve the number of passes for. 0 is always guaranteed
         *								to be the default technique.
         * @return						Number of passes used by the technique.
         */
        UINT32 GetNumPasses(UINT32 techniqueIdx = 0) const;

        /**
         * Retrieves a specific shader pass from the provided technique.
         *
         * @param[in]	passIdx			Sequential index of the pass to retrieve.
         * @param[in]	techniqueIdx	Index of the technique to retrieve the pass for. 0 is always guaranteed to be
         *								the default technique.
         * @return						Pass if found, null otherwise.
         */
        SPtr<Pass> GetPass(UINT32 passIdx = 0, UINT32 techniqueIdx = 0) const;

        /** Assigns a texture to the shader parameter with the specified name. */
        void SetTexture(const String& name, const SPtr<Texture>& value, const TextureSurface& surface = GpuParams::COMPLETE);

        /** Returns a pointer to the texture associated to "name". Returns nullptr if not exists */
        SPtr<Texture> GetTexture(const String& name);

        /** We can reset a texture on a material */
        void RemoveTexture(const String& name);

        /** Assigns a texture to the shader parameter with the specified name. */
        void SetLoadStoreTexture(const String& name, const SPtr<Texture>& value, const TextureSurface& surface = GpuParams::COMPLETE);

        /** Assigns a buffer to the shader parameter with the specified name. */
        void SetBuffer(const String& name, const SPtr<GpuBuffer>& value);

        /** Assigns a sampler state to the shader parameter with the specified name. */
        void SetSamplerState(const String& name, const SPtr<SamplerState>& value);

        /** Get sampler state */
        const SPtr<SamplerState>& GetSamplerState(const String& name);

        /** @copydoc Material::SetTexture */
        void SetTexture(const String& name, const HTexture& value, const TextureSurface& surface = GpuParams::COMPLETE);

        /** @copydoc Material::SetLoadStoreTexture */
        void SetLoadStoreTexture(const String& name, const HTexture& value, const TextureSurface& surface = GpuParams::COMPLETE);

        /** Assigns a value to an arbitrary constant buffer parameter. */
        template <typename T>
        void SetParam(const String& name, T& data, GpuProgramType programType = GpuProgramType::GPT_COUNT)
        {
            ParamData param;
            auto it = _params.find(name);

            if (it != _params.end())
                te_deallocate(it->second.Param);

            param.Param = te_allocate<T>(sizeof(T));
            param.Size = sizeof(T);
            param.ProgramType = programType;
            memcpy(param.Param, &data, param.Size);

            _params[name] = param;
        }

        /* Create all gpu params for a set of passes related to the current technique */
        void CreateGpuParams(UINT32 techniqueIdx, Vector<SPtr<GpuParams>>& outputParams);

        /** Here you can set all properties for a given material */
        const MaterialProperties& GetProperties() { return _properties; }

        /** ParamBlockBuffer are sometimes not currently set when creating gpuparams. So we give the ability to set manually gpu params */
        void SetGpuParam(SPtr<GpuParams> outparams);

        void SetProperties(const MaterialProperties& properties) 
        { 
            _properties = properties;
            _markCoreDirty(MaterialDirtyFlags::Param);
        }

    public:
        /** Creates a new empty material. */
        static HMaterial Create();

        /** Creates a new material with the specified shader. */
        static HMaterial Create(const HShader& shader);

        /** Creates a new material with the specified shader. */
        static HMaterial Create(const SPtr<Shader>& shader);

        /**	Creates a new empty material but doesn't initialize it. */
        static SPtr<Material> CreateEmpty();

        /** Marks the contents of the sim thread object as dirty, causing it to sync with its core thread counterpart. */
        virtual void _markCoreDirty(MaterialDirtyFlags flags = MaterialDirtyFlags::Param);

    protected:
        Material();
        Material(UINT32 id);
        Material(const HShader& shader, UINT32 id);
        Material(const SPtr<Shader>& shader, UINT32 id);
        Material(const HShader& shader, const Vector<SPtr<Technique>>& techniques, UINT32 id);
        Material(const SPtr<Shader>& shader, const Vector<SPtr<Technique>>& techniques, UINT32 id);

        /**
         * Initializes the material by using the compatible techniques from the currently set shader. Shader must contain
         * the techniques that matches the current renderer and render system.
         */
        void InitializeTechniques();

    protected:
        struct TextureData
        {
            TextureData()
                : TextureElem(nullptr)
                , TextureSurfaceElem(TextureSurface(0, 0, 0, 0))
            { }

            TextureData(const SPtr<Texture>& value, const TextureSurface& surface = GpuParams::COMPLETE)
                : TextureElem(value)
                , TextureSurfaceElem(surface)
            { }

            SPtr<Texture> TextureElem;
            TextureSurface TextureSurfaceElem;
        };

        struct ParamData
        {
            void* Param;
            size_t Size;
            GpuProgramType ProgramType;
        };

    protected:
        UINT32 _id;
        SPtr<Shader> _shader;
        Vector<SPtr<Technique>> _techniques;
        ShaderVariation _variation;

        UnorderedMap<String, SPtr<TextureData>> _textures;
        UnorderedMap<String, SPtr<TextureData>> _loadStoreTextures;
        UnorderedMap<String, SPtr<GpuBuffer>> _buffers;
        UnorderedMap<String, SPtr<SamplerState>> _samplerStates;
        UnorderedMap<String, ParamData> _params;

        MaterialProperties _properties;

        static std::atomic<UINT32> NextMaterialId;
    };
}
