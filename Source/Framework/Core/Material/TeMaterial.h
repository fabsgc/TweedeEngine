#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Mesh/TeMesh.h"

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

    /**
     * Material that controls how objects are rendered. It is represented by a shader and parameters used to set up that
     * shader. It provides a simple interface for manipulating the parameters.
     */
    class TE_CORE_EXPORT Material : public Resource
    {
    public:
        virtual ~Material() = default;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**
         * Sets a shader that will be used by the material. Material will be initialized using all compatible techniques
         * from the shader. Shader must be set before doing any other operations with the material.
         */
        void SetShader(const SPtr<Shader>& shader);

        /** Returns the currently active shader. */
        SPtr<Shader> GetShader() const { return _shader; } 
    
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
        Material(const HShader& shader);
        Material(const SPtr<Shader>& shader);

    protected:
        SPtr<Shader> _shader;
    };
}
