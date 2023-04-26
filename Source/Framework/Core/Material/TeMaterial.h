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

    /** Refraction type */
    enum class RefractionType : UINT32
    {
        Solid = 0, // refraction through solid objects (e.g. a sphere)
        Thin = 1, // refraction through thin objects (e.g. window)
    };

    /** Structure used when searching for a specific technique in a Material. */
    struct FIND_TECHNIQUE_DESC
    {
        /** A set of tags that the technique must have. */
        Vector<String> Tags;

        /** Number of valid tags in the @p tags array. */
        UINT32 NumTags = 0;

        /** Specified variation of the technique. Parameters not specified in the variation are assumed to be irrelevant. */
        ShaderVariation Variation;

        /**
         * Determines should the parameters in @p variation override any parameters that might have been defined on the
         * Material itself. If false then you are guaranteed to search only over the subset of techniques that match the
         * Material's internal variaton parameters. If true then you can search outside that range by setting a variation
         * parameter to some different value. Overriding can be useful for renderers which might need to override the user's
         * choice of variation.
         */
        bool Override = false;

        /** Registers a new tag to look for when searching for the technique. */
        void AddTag(const String& tag)
        {
            Tags.push_back(tag);
            NumTags++;
        }
    };

    /**
     * @brief 
     * 
     * Several material model properties expect RGB colors. 
     * The engine materials use RGB colors in linear space 
     * and you must take proper care of supplying colors in that space. 
     * 
     * Materials expect colors to use pre-multiplied alpha.
     * A color uses pre-multiplied alpha if its RGB components are multiplied by the alpha channel:
     * color.rgb *= color.a;
     * 
     * The light attenuation through the material is defined as e^(−absorption⋅distance), 
     * and the distance depends on the thickness parameter. 
     * If thickness is not provided, then the absorption parameter 
     * is used directly and the light attenuation through the material 
     * becomes 1−absorption. 
     * To obtain a certain color at a desired distance, 
     * the above equation can be inverted such as absorption=−ln(color)/distance.
     * 
     * Thickness represents the thickness of solid objects in the direction 
     * of the normal, for satisfactory results, this should be provided 
     * per fragment (e.g.: as a texture) or at least per vertex. 
     * MicroThickness represent the thickness of the thin layer of an object, 
     * and can generally be provided as a constant value. 
     * For example, a 1mm thin hollow sphere of radius 1m, 
     * would have a thickness of 1 and a microThickness of 0.001.
     * 
     * Note : If UseRoughnessMap or UseMetallicMap and UseMetallicRoughnessMap 
     * are both defined, UseMetallicRoughnessMap wins
     * 
     * Note : If both UseTransmissionMap and UseOpacityMap are defined, 
     * UseTransmissionMap wins
     */
    class MaterialProperties
    {
    public:
        /**
         * @brief Diffuse albedo for non-metallic surfaces, and specular color for metallic surfaces. 
         * Pre-multiplied linear RGB.
         * Alpha channel will not be used.
         * DiffuseColor = (1.0 - metallic) * BaseColor.rgb;
         * Range : [0..1]
         */
        Color BaseColor = Color(0.8f, 0.8f, 0.8f);

        /**
         * @brief Whether a surface appears to be dielectric (0.0) or conductor (1.0). 
         * Often used as a binary value (0 or 1).
         * Range : [0..1]
         */
        float Metallic = 0.05f;

        /**
         * @brief Perceived smoothness (1.0) or roughness (0.0) of a surface. 
         * Smooth surfaces exhibit sharp reflections.
         * Roughness = Roughness * Roughness
         * Range : [0..1]
         */
        float Roughness = 0.5f;

        /**
         * @brief Fresnel reflectance at normal incidence for dielectric surfaces.
         * This directly controls the strength of the reflections.
         * f0 = 0.16 * reflectance * reflectance * (1.0 - metallic) + baseColor * metallic;
         * f90 = 1.0
         * Range : [0..1]
         * Prefer values > 0.35
         */
        float Reflectance = 0.85f;

        /**
         * @brief Defines how much of the ambient light is accessible to a surface point. 
         * It is a per-pixel shadowing factor between 0.0 and 1.0. 
         * Only used on direct lighting.
         * Range : [0..1]
         */
        float Occlusion = 1.0f;

        /**
         * @brief Additional diffuse albedo to simulate emissive surfaces (such as neons, etc.) 
         * This property is mostly useful in an HDR pipeline with a bloom pass. 
         * Linear RGB intensity in nits
         * Range : [0..1]
         */
        Color Emissive = Color::Black;

        /**
         * @brief Strength of the sheen layer. Linear RGB
         * Range : [0..1]
         */
        Color SheenColor = Color::Black;

        /**
         * @brief Perceived smoothness or roughness of the sheen layer
         * Range : [0..1]
         */
        float SheenRoughness = 0.0f;

        /**
         * @brief Strength of the clear coat layer
         * Should be 0 or 1
         * Range : [0..1]
         */
        float ClearCoat = 0.0f;

        /**
         * @brief Perceived smoothness or roughness of the clear coat layer
         * Range : [0..1]
         */
        float ClearCoatRoughness = 0.0f;

        /**
         * @brief Tint for the diffuse color after scattering and absorption through the material. Linear RGB
         * Range : [0..1]
         */
        Color SubsurfaceColor = Color::White;

        /**
         * @brief Subsurface strength
         * Range : [0..n]
         */
        float SubsurfacePower = 12.0f;

        /**
         * @brief Amount of anisotropy in either the tangent or bitangent direction
         * Anisotropy is in the tangent direction when this value is positive
         * Range : [−1..1]
         */
        float Anisotropy = 0.0f;

        /**
         * @brief Local surface direction in tangent space
         * Linear RGB, encodes a direction vector in tangent space
         * Range : [0..1]
         */
        Vector3 AnisotropyDirection = Vector3::UNIT_X;

        /**
         * If alpha value (from transparency or transparency map)
         * is below a certain value, pixel will not be visible (discarded)
         * Range : [0..1]
         */
        float AlphaThreshold = 0.0f;

        /**
         * @brief Offset of any texture used for this material (UV0)
         */
        Vector2 TextureRepeat = Vector2::ONE;

        /**
         * @brief Repeat factor of any texture used for this material (UV0)
         */
        Vector2 TextureOffset = Vector2::ZERO;

        /**
         * @brief Controls parallax effect strength
         * Range : [0..1]
         */
        float ParallaxScale = 0.05f;

        /**
         * @brief Controls number of samples used for parallax mapping
         * Range : [16..256]
         */
        UINT32 ParallaxSamples = 64;

        /**
         * @brief Thickness of the thin layer of refractive objects
         * Range : [0..n]
         */
        float MicroThickness = 0.0f;

        /**
         * @brief MicroThickness of the thin volume of refractive objects
         * Range : [0..n]
         */
        float Thickness = 1.0f;

        /**
         * @brief Defines how much of the diffuse light of a dielectric is transmitted through the object, 
         * in other words this defines how transparent an object is
         * 0.0 = Opaque, 1.0 = Fully transparent
         * Range : [0..1]
         */
        float Transmission = 0.0f;

        /**
         * @brief Setting the absorption coefficients directly can be unintuitive which is why we recommend 
         * working with a transmittance color and a “at distance” factor instead. These two parameters 
         * allow an artist to specify the precise color the material should have at a specified distance 
         * through the volume. The value to pass to absorption can be computed this way: 
         * absoption = (-ln(transmittanceColor)/atDistance)
         */
        Color TransmittanceColor = Color::White;

        /**
         * @copydoc TransmittanceColor
         */
        float AtDistance = 1.0f;

        /**
         * @brief Absorption factor for refractive objects
         * Range : [0..n]
         */
        Vector3 Absorption = Vector3::ZERO;

        /**
         * @brief Refraction type (Solid or Thin)
         */
        RefractionType RefractType = RefractionType::Solid;

        /**
         * @copydoc MaterialProperties::BaseColor
         * If an alpha channel is provided, it will be used for transmission
         */
        bool UseBaseColorMap = false;

        /**
         * @copydoc MaterialProperties::Metallic
         */
        bool UseMetallicMap = false;

        /**
         * @copydoc MaterialProperties::Roughness
         */
        bool UseRoughnessMap = false;

        /**
         * @copydoc MaterialProperties::Metallic
         * @copydoc MaterialProperties::Roughness
         * From gltf specification, we can use a texture with both information
         * B : Metallic, G : Roughness. R and A : not defined
         */
        bool UseMetallicRoughnessMap = false;

        /**
         * @copydoc MaterialProperties::Reflectance
         */
        bool UseReflectanceMap = false;
        
        /**
         * @copydoc MaterialProperties::Occlusion
         */
        bool UseOcclusionMap = false;

        /**
         * @copydoc MaterialProperties::Emissive
         */
        bool UseEmissiveMap = false;

        /**
         * @copydoc MaterialProperties::SheenColor
         */
        bool UseSheenColorMap = false;

        /**
         * @copydoc MaterialProperties::SheenRoughness
         */
        bool UseSheenRoughnessMap = false;

        /**
         * @copydoc MaterialProperties::ClearCoat
         */
        bool UseClearCoatMap = false;

        /**
         * @copydoc MaterialProperties::ClearCoatRoughness
         */
        bool UseClearCoatRoughnessMap = false;

        /**
         * @brief A detail normal used to perturb the clear coat 
         * layer using bump mapping (normal mapping)
         * Linear RGB, encodes a direction vector in tangent space
         * Range : [0..1]
         */
        bool UseClearCoatNormalMap = false;

        /**
         * @brief A detail normal used to perturb the surface
         * using bump mapping (normal mapping)
         * Linear RGB, encodes a direction vector in tangent space
         * Range : [0..1]
         */
        bool UseNormalMap = false;

        /**
         * @copydoc MaterialProperties::ParallaxScale
         */
        bool UseParallaxMap = false;

        /**
         * @copydoc MaterialProperties::Transmission
         */
        bool UseTransmissionMap = false;

        /**
         * @copydoc MaterialProperties::Transmission
         * Only R channel is used
         * 0 : transparent
         * 1 : Opaque
         * Range : [0..1]
         * Transmission = 1 - Opacity
         */
        bool UseOpacityMap = false;

        /**
         * @copydoc MaterialProperties::AnisotropyDirection
         */
        bool UseAnisotropyDirectionMap = false;

        /**
         * @brief Specify if this material must compute IBL using textures
         * coming from this material, skybox or light probe
         * Priority : Material > Light Probe > Skybox
        */
        bool DoIndirectLighting = true;

        /**
         * @brief You must call this method if TransmittanceColor or AtDistance
         * are modified in order to update Absoption
         */
        void ComputeAbsorption()
        {
            Absorption = TransmittanceColor.GetAsVector3();
            Absorption.x = -Math::Log(Math::Clamp(Absorption.x, 1e-5f, 1.0f));
            Absorption.y = -Math::Log(Math::Clamp(Absorption.y, 1e-5f, 1.0f));
            Absorption.z = -Math::Log(Math::Clamp(Absorption.z, 1e-5f, 1.0f));
            Absorption /= Math::Max(1e-5f, AtDistance);
        }

        /**
         * @brief Construct a new Material Properties object
         */
        MaterialProperties()
        {
            ComputeAbsorption();
        }
    };

    /**
     * @brief Mostly used by editor to fill textures path from files
     */
    struct MaterialTextures
    {
        String BaseColorMap;
        String MetallicMap;
        String RoughnessMap;
        String MetallicRoughnessMap;
        String ReflectanceMap;
        String OcclusionMap;
        String EmissiveMap;
        String SheenColorMap;
        String SheenRoughnessMap;
        String ClearCoatMap;
        String ClearCoatRoughnessMap;
        String ClearCoatNormalMap;
        String NormalMap;
        String ParallaxMap;
        String TransmissionMap;
        String OpacityMap;
        String AnisotropyDirectionMap;
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
         * FindTechnique().
         */
        void SetVariation(const ShaderVariation& variation);

        /** Returns the currently active shader. */
        SPtr<Shader> GetShader() const { return _shader; } 

        /**
         * Set of parameters that determine which subset of techniques in the assigned shader should be used. Only the
         * techniques that have the provided parameters with the provided values will match. This will control which
         * technique is considered the default technique and which subset of techniques are searched during a call to
         * FindTechnique().
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
         * @param[in]	createTechnique		if no technique has been found, try to create a valid one
         * @return							First technique that matches the tags & variation parameters specified in
         *									@p desc.
         */
        UINT32 FindTechnique(const FIND_TECHNIQUE_DESC& desc, bool createTechnique = false) const;

        /**
         * Finds the index of the default (primary) technique to use. This will be the first technique that matches the
         * currently set variation parameters (if any).
         * 
         * @param[in]   createTechnique		if no technique has been found, try to create a valid one
         */
        UINT32 GetDefaultTechnique(bool createTechnique = false) const;

        /**
         * Create a technique using the description
         * 
         */
        UINT32 CreateTechnique(const FIND_TECHNIQUE_DESC& desc);

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

        /** Creates a new material with the specified shader . */
        static HMaterial Create(const HShader& shader);

        /** Creates a new material with the specified shader. */
        static HMaterial Create(const SPtr<Shader>& shader);

        /** Creates a new material with the specified shader and variation. */
        static HMaterial Create(const HShader& shader, const ShaderVariation& variation);

        /** Creates a new material with the specified shader and variation. */
        static HMaterial Create(const SPtr<Shader>& shader, const ShaderVariation& variation);

        /**	Creates a new empty material but doesn't initialize it. */
        static SPtr<Material> CreateEmpty();

        /** Marks the contents of the sim thread object as dirty, causing it to sync with its core thread counterpart. */
        virtual void _markCoreDirty(MaterialDirtyFlags flags = MaterialDirtyFlags::Param);

    protected:
        Material();
        Material(UINT32 id, const ShaderVariation& variation);
        Material(const HShader& shader, const ShaderVariation& variation, UINT32 id);
        Material(const SPtr<Shader>& shader, const ShaderVariation& variation, UINT32 id);

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
        mutable Map<UINT32, SPtr<Technique>> _techniques;
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
