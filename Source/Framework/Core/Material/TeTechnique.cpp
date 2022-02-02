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
    Technique::Technique()
        : Resource(TID_Technique)
        , _variation(ShaderVariation::EMPTY)
    { }

    Technique::~Technique()
    { }

    void Technique::Initialize()
    {
        CoreObject::Initialize();
    }

    Technique::Technique(const String& language, const Vector<StringID>& tags, 
        const ShaderVariation& variation, const Vector<SPtr<Pass>>& passes)
        : Resource(TID_Technique)
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

    bool Technique::HasTag(const StringID& tag)
    {
        for (auto& entry : _tags)
        {
            if (entry == tag)
                return true;
        }

        return false;
    }

    void Technique::Compile()
    {
        for (auto& pass : _passes)
            pass->Compile();
    }

    SPtr<Pass> Technique::GetPass(UINT32 idx) const
    {
        if (idx >= (UINT32)_passes.size())
            TE_ASSERT_ERROR(false, "Index out of range: " + ToString(idx));

        return _passes[idx];
    }

    HTechnique Technique::Create(const String& language, const Vector<SPtr<Pass>>& passes)
    {
        const SPtr<Technique> techniquePtr = CreatePtr(language, passes);
        techniquePtr->Initialize();

        return static_resource_cast<Technique>(gResourceManager()._createResourceHandle(techniquePtr));
    }

    HTechnique Technique::Create(const String& language, const Vector<StringID>& tags,
        const ShaderVariation& variation, const Vector<SPtr<Pass>>& passes)
    {
        const SPtr<Technique> techniquePtr = CreatePtr(language, passes);
        techniquePtr->Initialize();

        return static_resource_cast<Technique>(gResourceManager()._createResourceHandle(techniquePtr));
    }

    SPtr<Technique> Technique::CreatePtr(const String& language, const Vector<SPtr<Pass>>& passes)
    {
        Technique* technique = new (te_allocate<Technique>()) Technique(language, {}, ShaderVariation::EMPTY, passes);
        SPtr<Technique> techniquePtr = te_core_ptr<Technique>(technique);
        techniquePtr->SetThisPtr(techniquePtr);
        techniquePtr->Initialize();

        return techniquePtr;
    }

    SPtr<Technique> Technique::CreatePtr(const String& language, const Vector<StringID>& tags,
        const ShaderVariation& variation, const Vector<SPtr<Pass>>& passes)
    {
        Technique* technique = new (te_allocate<Technique>()) Technique(language, tags, variation, passes);
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
