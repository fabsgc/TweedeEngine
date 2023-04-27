#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeGpuParams.h"
#include "Renderer/TeRendererMaterialManager.h"
#include "Material/TeShader.h"
#include "Material/TePass.h"
#include "Material/TeTechnique.h"
#include "RenderAPI/TeRenderAPI.h"

#if TE_COMPILER == TE_COMPILER_MSVC
#   pragma warning (push)
#   pragma warning (disable: 4702)
#endif

namespace te
{
    /** References the shader path in RendererMaterial implementation. */
#define RMAT_DEF(type)														\
    public:																	\
    static void InitMetaData()												\
    {																		\
        te::RendererMaterialManager::RegisterMaterial(&_metaData, type);	\
    };																		\

/**
 * References the shader path in RendererMaterial implementation. Provides an _initDefines() method allowing the C++
 * code to provide preprocessor defines to be set when compiling the shader. Note that when changing these defines you need
 * to manually force the shader to be reimported.
 */
#define RMAT_DEF_CUSTOMIZED(type)											\
    public:																	\
    static void InitMetaData()												\
    {																		\
        InitDefines(_metaData.Defines);										\
        te::RendererMaterialManager::RegisterMaterial(&_metaData, type);	\
    };																		\
    static void InitDefines(ShaderDefines& defines);

    struct RendererMaterialMetaData
    {
        SPtr<Shader> ShaderElem = nullptr;
        Vector<RendererMaterialBase*> Instances;
        ShaderVariations Variations;
        ShaderDefines Defines;
    };

    /**	Helper class to initialize all renderer materials as soon as the library is loaded. */
    template <class T>
    struct InitRendererMaterialStart
    {
    public:
        InitRendererMaterialStart()
        {
            T::InitMetaData();
        }

        /**	Forces the compiler to not optimize out construction of this type. */
        void Instantiate() { }
    };

    /**	Base class for all RendererMaterial instances, containing common data and methods. */
    class TE_CORE_EXPORT RendererMaterialBase : public NonCopyable
    {
    public:
        virtual ~RendererMaterialBase() = default;

        /** Initialize */
        virtual void Initialize() { }

        /** Returns the shader used by the material. */
        SPtr<Shader> GetShader() const { return _shader; }

        /** Returns the internal parameter set containing GPU bindable parameters. */
        SPtr<GpuParams> GetParams() const { return _params; }

        /**
         * Helper field to be set before construction. Identifiers the variation of the material to initialize this
         * object with.
         */
        UINT32 _varIdx;

    public:
        void InitPipelines()
        {
            if(_shader == nullptr)
            {
                TE_DEBUG("Shader is NULL, can't create RendererMaterial");
                return;
            }

            const Vector<SPtr<Technique>>& techniques = _shader->GetTechniques();
            for (auto& entry : techniques)
            {
                if(!entry->IsSupported())
                    continue;

                if(entry->GetVariation() == _variation)
                {
                    SPtr<Pass> pass = std::const_pointer_cast<Pass>(entry->GetPass(0));
                    pass->Compile(_variation);

                    _graphicsPipeline = pass->GetGraphicsPipelineState();
                    if (_graphicsPipeline != nullptr)
                    {
                        _params = GpuParams::Create(_graphicsPipeline);
                    }
                    else
                    {
                        _computePipeline = pass->GetComputePipelineState();
                        _params = GpuParams::Create(_computePipeline);
                    }

                    // Assign default values from the shader
                    const auto& textureParams = _shader->GetTextureParams();
                    for (auto& param : textureParams)
                    {
                        UINT32 defaultValueIdx = param.second.DefaultValueIdx;
                        if (defaultValueIdx == (UINT32)-1)
                            continue;

                        for (UINT32 i = 0; i < GPT_COUNT; i++)
                        {
                            GpuProgramType progType = (GpuProgramType)i;
                            for (auto& varName : param.second.GpuVariableNames)
                            {
                                if (_params->HasTexture(progType, varName))
                                {
                                    SPtr<Texture> texture = _shader->GetDefaultTexture(defaultValueIdx);
                                    _params->SetTexture(progType, varName, texture);
                                }
                            }
                        }
                    }

                    const auto& samplerParams = _shader->GetSamplerParams();
                    for (auto& param : samplerParams)
                    {
                        UINT32 defaultValueIdx = param.second.DefaultValueIdx;
                        if (defaultValueIdx == (UINT32)-1)
                            continue;

                        for (UINT32 i = 0; i < GPT_COUNT; i++)
                        {
                            GpuProgramType progType = (GpuProgramType)i;
                            for (auto& varName : param.second.GpuVariableNames)
                            {
                                if (_params->HasSamplerState(progType, varName))
                                {
                                    SPtr<SamplerState> samplerState = _shader->GetDefaultSampler(defaultValueIdx);
                                    _params->SetSamplerState(progType, varName, samplerState);
                                }
                            }
                        }
                    }

                    _stencilRef = pass->GetStencilRefValue();
                }
            }
        }

    protected:
        friend class RendererMaterialManager;

        SPtr<GpuParams> _params;
        SPtr<GraphicsPipelineState> _graphicsPipeline;
        SPtr<ComputePipelineState> _computePipeline;
        UINT32 _stencilRef = 0;

        ShaderVariation _variation;
        SPtr<Shader> _shader;
    };

    /** Wrapper class around Material that allows a simple way to load and set up materials used by the renderer. */
    template<class T>
    class RendererMaterial : public RendererMaterialBase
    {
    public:
        virtual ~RendererMaterial() = default;

        /**
         * Retrieves an instance of this renderer material. If material has multiple variations the first available
         * variation will be returned.
         */
        static T* Get()
        {
            if(_metaData.Instances[0] == nullptr)
            {
                RendererMaterialBase* mat = te_allocate<T>();
                mat->_varIdx = 0;
                new (mat) T();
                mat->Initialize();

                _metaData.Instances[0] = mat;
            }

            return (T*)_metaData.Instances[0];
        }

        /** Retrieves an instance of a particular variation of this renderer material. */
        static T* Get(const ShaderVariation& variation)
        {
            if(variation.GetIdx() == (UINT32)-1)
                variation.SetIdx(_metaData.Variations.Find(variation));

            UINT32 varIdx = variation.GetIdx();

            TE_ASSERT_ERROR((variation.GetIdx() != (UINT32)-1), "Failed to find a compatible variation for this material")

            if(_metaData.Instances[varIdx] == nullptr)
            {
                RendererMaterialBase* mat = te_allocate<T>();
                mat->_varIdx = varIdx;
                new (mat) T();
                mat->Initialize();

                _metaData.Instances[varIdx] = mat;
            }

            return (T*)_metaData.Instances[varIdx];
        }

        /**
         * Binds the materials and its parameters to the pipeline. This material will be used for rendering any subsequent
         * draw calls, or executing dispatch calls. If @p bindParams is false you need to call bindParams() separately
         * to bind material parameters (if any).
         */
        void Bind(bool bindParams = true) const
        {
            RenderAPI& rapi = RenderAPI::Instance();

            if (_graphicsPipeline)
            {
                rapi.SetGraphicsPipeline(_graphicsPipeline);
                rapi.SetStencilRef(_stencilRef);
            }
            else if(_computePipeline)
            {
                rapi.SetComputePipeline(_computePipeline);
            }

            if (bindParams)
                rapi.SetGpuParams(_params);
        }

        /** Binds the material parameters to the pipeline. */
        void BindParams() const
        {
            if(_params)
            {
                RenderAPI& rapi = RenderAPI::Instance();
                rapi.SetGpuParams(_params);
            }
        }

    protected:
        RendererMaterial()
        {
            _initOnStart.Instantiate();
            _shader = _metaData.ShaderElem;

            if(_metaData.Variations.Exist(_varIdx))
                _variation = _metaData.Variations.Get(_varIdx);

            InitPipelines();
        }

        /** Returns a set of dynamically defined defines used when compiling this shader. */
        static ShaderDefines GetShaderDefines() { return _metaData.Defines; }

    protected:
        static RendererMaterialMetaData _metaData;
        static InitRendererMaterialStart<T> _initOnStart;
    };

    template<class T>
    InitRendererMaterialStart<T> RendererMaterial<T>::_initOnStart;

    template<class T>
    RendererMaterialMetaData RendererMaterial<T>::_metaData;
}

#if TE_COMPILER == TE_COMPILER_MSVC
#   pragma warning (pop)
#endif
