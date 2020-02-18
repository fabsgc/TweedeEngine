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
        SPtr<Shader> FetShader() const { return _shader; }

        /** Returns the internal parameter set containing GPU bindable parameters. */
        SPtr<GpuParams> FetParams() const { return _params; }

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

    protected:
        RendererMaterial()
        {
            _initOnStart.Instantiate();
            _shader = _metaData.ShaderElem;
            const Vector<SPtr<Technique>>& techniques = _shader->GetTechniques();
            for (auto& entry : techniques)
            {
                // TODO
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
