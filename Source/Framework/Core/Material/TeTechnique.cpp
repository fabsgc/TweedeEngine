#include "TeTechnique.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Manager/TeRendererManager.h"
#include "Renderer/TeRenderer.h"
#include "TePass.h"

namespace te
{
    Technique::Technique()
    { }

    Technique::Technique(const String& language, const Vector<SPtr<Pass>>& passes)
        : _language(language)
        , _passes(passes)
    { }

    bool Technique::IsSupported() const
    {
        if (GpuProgramManager::Instance().IsLanguageSupported(_language) || _language == "any")
            return true;

        return false;
    }

    void Technique::Compile()
    {
        for (auto& pass : _passes)
            pass->Compile();
    }

    SPtr<Pass> Technique::GetPass(UINT32 idx) const
    {
        if (idx < 0 || idx >= (UINT32)_passes.size())
            TE_ASSERT_ERROR(false, "Index out of range: " + ToString(idx), __FILE__, __LINE__);

        return _passes[idx];
    }

    SPtr<Technique> Technique::Create(const String& language, const Vector<SPtr<Pass>>& passes)
    {
        Technique* technique = new (te_allocate<Technique>()) Technique(language, passes);
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

    void Technique::FrameSync()
    { }
}
