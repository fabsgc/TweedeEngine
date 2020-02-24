#pragma once

#include "TeCorePrerequisites.h"
#include "Material/TeMaterial.h"
#include "Renderer/TeRendererMaterialManager.h"
#include "Material/TeShader.h"
#include "Material/TePass.h"
#include "RenderAPI/TeRenderAPI.h"

namespace te
{
    /** References the shader path in RendererMaterial implementation. */
#define RMAT_DEF(type)														\
    public:																	\
    static void _initMetaData()												\
    {																		\
    	te::RendererMaterialManager::_registerMaterial(&_metaData, type);	\
    };																		\

    struct RendererMaterialMetaData
    {
        SPtr<Shader> ShaderElem;
        RendererMaterialBase* Instance;
    };

    /**	Helper class to initialize all renderer materials as soon as the library is loaded. */
    template <class T>
    struct InitRendererMaterialStart
    {
    public:
        InitRendererMaterialStart()
        {
            T::_initMetaData();
        }

        /**	Forces the compiler to not optimize out construction of this type. */
        void Instantiate() { }
    };

    /**	Base class for all RendererMaterial instances, containing common data and methods. */
    class TE_CORE_EXPORT RendererMaterialBase
    {
    public:
        virtual ~RendererMaterialBase() = default;

        /** Returns the shader used by the material. */
        SPtr<Shader> GetShader() const { return _shader; }

        /** Returns the internal parameter set containing GPU bindable parameters. */
        SPtr<GpuParams> GetParams() const { return _params; }

    protected:
        friend class RendererMaterialManager;

        SPtr<GpuParams> _params;
        SPtr<GraphicsPipelineState> _graphicsPipeline;
        SPtr<ComputePipelineState> _computePipeline;
        UINT32 _stencilRef = 0;

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
            if (_metaData.Instance == nullptr)
            {
                RendererMaterialBase* mat = te_allocate<T>();
                new (mat) T();

                _metaData.Instance = mat;
            }

            return (T*)_metaData.Instance;
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
            else
                rapi.SetComputePipeline(_computePipeline);

            if (bindParams)
                rapi.SetGpuParams(_params);
        }

        /** Binds the material parameters to the pipeline. */
        void BindParams() const
        {
            RenderAPI& rapi = RenderAPI::Instance();
            rapi.SetGpuParams(_params);
        }

    protected:
        RendererMaterial()
        {
            _initOnStart.Instantiate();
            _shader = _metaData.ShaderElem;

            const Vector<SPtr<Technique>>& techniques = _shader->GetTechniques();
            for (auto& entry : techniques)
            {
                SPtr<Pass> pass = entry->GetPass(0);
                pass->Compile();

                _graphicsPipeline = pass->GetGraphicsPipelineState();
                if (_graphicsPipeline != nullptr)
                    _params = GpuParams::Create(_graphicsPipeline);
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

    protected:
        static RendererMaterialMetaData _metaData;
        static InitRendererMaterialStart<T> _initOnStart;
    };

    template<class T>
    InitRendererMaterialStart<T> RendererMaterial<T>::_initOnStart;

    template<class T>
    RendererMaterialMetaData RendererMaterial<T>::_metaData;
}
