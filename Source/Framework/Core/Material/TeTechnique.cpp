#include "TeTechnique.h"
#include "Renderer/TeRenderer.h"
#include "RenderAPI/TeRenderAPI.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "Manager/TeRendererManager.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"
#include "TePass.h"

namespace te
{
    std::atomic<UINT32> Technique::NextTechniqueId;

    Technique::Technique()
        : Serializable(TID_Technique)
        , _id(0)
    { 
        UINT32 id = Technique::NextTechniqueId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many techniques, reached maximum id.");

        _id = id;
    }

    Technique::Technique(UINT32 id, const String& language, const Vector<String>& tags, 
        const ShaderVariation& variation, const Vector<SPtr<Pass>>& passes)
        : Serializable(TID_Technique)
        , _id(id)
        , _language(language)
        , _tags(tags)
        , _variation(variation)
        , _passes(passes)
    { }

    bool Technique::IsSupported() const
    {
        if (GpuProgramManager::Instance().IsLanguageSupported(_language) || _language == "any")
            return true;

        return false;
    }

    bool Technique::HasTag(const String& tag)
    {
        for (auto& entry : _tags)
        {
            if (entry == tag)
                return true;
        }

        return false;
    }

    void Technique::Compile(bool force)
    {
        for (auto& pass : _passes)
            pass->Compile(_variation, force);
    }

    SPtr<Pass> Technique::GetPass(UINT32 idx) const
    {
        if (idx >= (UINT32)_passes.size())
            TE_ASSERT_ERROR(false, "Index out of range: " + ToString(idx));

        return _passes[idx];
    }

    const Vector<SPtr<Pass>>& Technique::GetPasses() const
    {
        return _passes;
    }

    SPtr<Technique> Technique::Create(const String& language, const Vector<SPtr<Pass>>& passes)
    {
        UINT32 id = Technique::NextTechniqueId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many techniques, reached maximum id.");

        Technique* technique = new (te_allocate<Technique>()) Technique(id, language, {}, ShaderVariation(), passes);
        SPtr<Technique> techniquePtr = te_core_ptr<Technique>(technique);
        techniquePtr->SetThisPtr(techniquePtr);
        techniquePtr->Initialize();

        return techniquePtr;
    }

    SPtr<Technique> Technique::Create(const String& language, const Vector<String>& tags,
        const ShaderVariation& variation, const Vector<SPtr<Pass>>& passes)
    {
        UINT32 id = Technique::NextTechniqueId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many techniques, reached maximum id.");

        Technique* technique = new (te_allocate<Technique>()) Technique(id, language, tags, variation, passes);
        SPtr<Technique> techniquePtr = te_core_ptr<Technique>(technique);
        techniquePtr->SetThisPtr(techniquePtr);
        techniquePtr->Initialize();

        return techniquePtr;
    }

    SPtr<Technique> Technique::CreateEmpty()
    {
        Technique* technique = new (te_allocate<Technique>()) Technique();
        SPtr<Technique> techniquePtr = te_core_ptr<Technique>(technique);
        techniquePtr->SetThisPtr(techniquePtr);

        return techniquePtr;
    }
}
