#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Mesh/TeMesh.h"
#include "TeTechnique.h"
#include "TePass.h"

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
        Material(const HShader& shader);
        Material(const SPtr<Shader>& shader);
        Material(const HShader& shader, const Vector<SPtr<Technique>>& techniques);
        Material(const SPtr<Shader>& shader, const Vector<SPtr<Technique>>& techniques);

        /**
         * Initializes the material by using the compatible techniques from the currently set shader. Shader must contain
         * the techniques that matches the current renderer and render system.
         */
        void InitializeTechniques();

    protected:
        SPtr<Shader> _shader;
        Vector<SPtr<Technique>> _techniques;
    };
}
