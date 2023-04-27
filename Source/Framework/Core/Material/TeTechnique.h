#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Material/TeShaderVariation.h"

#include <atomic>

namespace te
{
    /**
     * Technique is a set of shading passes bindable to the GPU pipeline. Each technique can also have a set of properties
     * that help the engine to determine which technique should be used under which circumstances (if more than one
     * technique is available).
     *
     * @note	
     * Normally you want to have a separate technique for every render system and renderer your application supports.
     * For example, if you are supporting DirectX11 and OpenGL you will want to have two techniques, one using HLSL based
     * GPU programs, other using GLSL. Those techniques should try to mirror each other's end results.
     */
    class TE_CORE_EXPORT Technique : public CoreObject, public Serializable, public NonCopyable
    {
    public:
        virtual ~Technique() = default;

        /** Returns the unique technique ID. */
        UINT32 GetId() const { return _id; }

        /**	Checks if this technique is supported based on current render and other systems. */
        bool IsSupported() const;

        /** Checks if the technique has the specified tag. */
        bool HasTag(const String& tag);

        /** Checks if the technique has any tags. */
        UINT32 HasTags() const { return !_tags.empty(); }

        /** Returns all tags */
        const Vector<String>& GetTags() const { return _tags; }

        /** Returns a set of preprocessor defines used for compiling this particular technique. */
        const ShaderVariation& GetVariation() const { return _variation; }

        /**	Returns a pass with the specified index. */
        const SPtr<Pass> GetPass(UINT32 idx) const;

        /**	Returns all passes */
        const Vector<SPtr<Pass>>& GetPasses() const;

        /**	Returns total number of passes. */
        UINT32 GetNumPasses() const { return (UINT32)_passes.size(); }

        /** Compiles all the passes in a technique. @see Pass::compile. */
        void Compile(bool force = false);

        /** Get shader language used by this technique */
        const String& GetLanguage() const { return _language; }

        /**
         * Creates a new technique.
         *
         * @param[in]	language	Shading language used by the technique. The engine will not use this technique unless
         *							this language is supported by the render API.
         * @param[in]	passes		A set of passes that define the technique.
         * @return					Newly creted technique.
         */
        static SPtr<Technique> Create(const String& language, const Vector<SPtr<Pass>>& passes);

        /**
         * Creates a new technique.
         *
         * @param[in]	language	Shading language used by the technique. The engine will not use this technique unless
         *							this language is supported by the render API.
         * @param[in]	tags		An optional set of tags that can be used for further identifying under which
         *							circumstances should a technique be used.
         * @param[in]	variation	A set of preprocessor directives that were used for compiling this particular technique.
         *							Used for shaders that have multiple variations.
         * @param[in]	passes		A set of passes that define the technique.
         * @return					Newly creted technique.
         */
        static SPtr<Technique> Create(const String& language, const Vector<String>& tags,
            const ShaderVariation& variation, const Vector<SPtr<Pass>>& passes);

        /**	Creates a new technique but doesn't initialize it. */
        static SPtr<Technique> CreateEmpty();

    private:
        Technique();
        Technique(UINT32 id, const String& language, const Vector<String>& tags, 
            const ShaderVariation& variation, const Vector<SPtr<Pass>>& passes);

    protected:
        UINT32 _id;
        String _language;
        Vector<String> _tags;
        ShaderVariation _variation;
        Vector<SPtr<Pass>> _passes;

        static std::atomic<UINT32> NextTechniqueId;
    };
}
