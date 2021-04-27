#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "TeTechnique.h"
#include "TePass.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "RenderAPI/TeGpuParams.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeGpuBuffer.h"
#include "RenderAPI/TeSamplerState.h"
#include "Image/TeColor.h"

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

    struct MaterialProperties
    {
        /** Basic parameters for controlling color of the material */
        Color Ambient = Color(0.75f, 0.75f, 0.75f, 0.5f);
        Color Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
        Color Emissive = Color(0.0f, 0.0f, 0.0f, 1.0f);
        Color Specular = Color(1.0f, 1.0f, 1.0f, 0.5f);

        /** Offsetting and Repeating of any texture used for this material */
        Vector2 TextureRepeat = Vector2(1.0f, 1.0f);
        Vector2 TextureOffset = Vector2(0.0f, 0.0f);

        /** Controls global specular properties */
        float SpecularPower = 64.0f;
        float SpecularStrength = 1.0f;

        /** Controls global transparency */
        float Transparency = 1.0f;

        /** 
         * Controls Reflection and Refraction strenght. 
         * If no EnvironmentMap is set, it will try to use skybox texture instead.
         */
        float IndexOfRefraction = 1.0f;
        float Refraction = 0.0f;
        float Reflection = 0.0f;

        /** Controls Bump map strenght */
        float BumpScale = 1.0f;

        /** Controls Parallax map strength */
        float ParallaxScale = 0.05f;

        /**
         * If alpha value (from transparency, diffuse map or transparency map)
         * is below a certain value, pixel will not be visible
         */
        float AlphaThreshold = 0.0f;

        /**
         * Albedo color texture. If this texture has an alpha channel, 
         * it will use it for transparency
         */
        bool UseDiffuseMap = false;

        /** Emissive color texture */
        bool UseEmissiveMap = false;

        /** Normals texture */
        bool UseNormalMap = false;

        /** Specular texture. Disable SpecularPower ans SpecularStrength  */
        bool UseSpecularMap = false;

        /** Bump mapping texture */
        bool UseBumpMap = false;

        /** Parallax mapping texture */
        bool UseParallaxMap = false;

        /** Transparency map. Disable transparency by global parameter or diffuse map */
        bool UseTransparencyMap = false;

        /** Reflection map (not implemented yet) */
        bool UseReflectionMap = false;

        /** Occlusion map */
        bool UseOcclusionMap = false;

        /**
         * Specify is a CubeMap texture is used for environment mapping. 
         * Useful for Reflection, Refraction and DynamicEnvironmentMapping
         */
        bool UseEnvironmentMap = false;

        /**
         * Specify is a CubeMap texture is used for global illumination. Useful for Global illumination
         */
        bool UseIrradianceMap = false;

        /**
         * Does this material must be illuminated using Global Illumination
         * Global Illumination will use by default Skybox irradiance texture if available in the scene
         * or material IrradianceMap otherwise. If no texture can be found, this step will not be performed
         */
        bool UseGlobalIllumination = true;

        /** 
         * If this is set to true, engine will try to find an available LightProbe or generate a custom CubeMap 
         * and assign this on "EnvironmentMap" texture. Disable EnvionmentMap texture
         */
        bool UseDynamicEnvironmentMap = false;
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
        ~Material();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_Material; }

        /** Returns the unique shader ID. */
        UINT32 GetId() const { return _id; }

        /**
         * Sets a shader that will be used by the material. Material will be initialized using all compatible techniques
         * from the shader. Shader must be set before doing any other operations with the material.
         */
        void SetShader(const SPtr<Shader>& shader);

        /** Returns the currently active shader. */
        SPtr<Shader> GetShader() const { return _shader; } 

        /** Returns the total number of techniques supported by this material. */
        UINT32 GetNumTechniques() const { return (UINT32)_techniques.size(); }

        /** Returns the technique at the specified index. */
        const SPtr<Technique>& GetTechnique(UINT32 idx) const { return _techniques[idx]; }

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
        void SetTexture(const String& name, const HTexture& value, const TextureSurface& surface = GpuParams::COMPLETE)
        {
            SetTexture(name, value.GetInternalPtr(), surface);
        }

        /** @copydoc Material::SetLoadStoreTexture */
        void SetLoadStoreTexture(const String& name, const HTexture& value, const TextureSurface& surface = GpuParams::COMPLETE)
        {
            SetLoadStoreTexture(name, value.GetInternalPtr(), surface);
        }

        /** Assigns a value to an arbitrary constant buffer parameter. */
        template <typename T>
        void SetParam(const String& name, T& data)
        {
            ParamData param;
            param.Param = te_allocate<T>(sizeof(T));
            param.Size = sizeof(T);
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

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

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
        };

    protected:
        UINT32 _id;
        SPtr<Shader> _shader;
        Vector<SPtr<Technique>> _techniques;

        UnorderedMap<String, SPtr<TextureData>> _textures;
        UnorderedMap<String, SPtr<TextureData>> _loadStoreTextures;
        UnorderedMap<String, SPtr<GpuBuffer>> _buffers;
        UnorderedMap<String, SPtr<SamplerState>> _samplerStates;
        UnorderedMap<String, ParamData> _params;

        MaterialProperties _properties;

        static std::atomic<UINT32> NextMaterialId;
    };
}
