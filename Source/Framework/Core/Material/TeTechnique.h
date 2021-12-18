#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    class TE_CORE_EXPORT Technique : public Resource
    {
    public:
        virtual ~Technique();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TypeID_Core::TID_Technique; }

        /**	Checks if this technique is supported based on current render and other systems. */
        bool IsSupported() const;

        /**	Returns a pass with the specified index. */
        SPtr<Pass> GetPass(UINT32 idx) const;

        /**	Returns total number of passes. */
        UINT32 GetNumPasses() const { return (UINT32)_passes.size(); }

        /** Compiles all the passes in a technique. @see Pass::compile. */
        void Compile();

        /**
         * Creates a new technique.
         *
         * @param[in]	language	Shading language used by the technique. The engine will not use this technique unless
         *							this language is supported by the render API.
         * @param[in]	passes		A set of passes that define the technique.
         * @return					Newly creted technique.
         */
        static HTechnique Create(const String& language, const Vector<SPtr<Pass>>& passes);

        /** @copydoc Technique::Create */
        static SPtr<Technique> CreatePtr(const String& language, const Vector<SPtr<Pass>>& passes);

        /**	Creates a new technique but doesn't initialize it. */
        static SPtr<Technique> CreateEmpty();

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    private:
        Technique();
        Technique(const String& language, const Vector<SPtr<Pass>>& passes);

    protected:
        String _language;
        Vector<SPtr<Pass>> _passes;
    };
}
