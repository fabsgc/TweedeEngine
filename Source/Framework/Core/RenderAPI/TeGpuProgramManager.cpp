#include "TeGpuProgramManager.h"
#include "RenderAPI/TeGpuProgram.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(GpuProgramManager)

    String sNullLang = "null";

    /** Null GPU program used in place of GPU programs we cannot create. Null programs don't do anything. */
    class NullProgram final : public GpuProgram
    {
    public:
        NullProgram()
            : GpuProgram(GPU_PROGRAM_DESC(), GDF_DEFAULT)
        { }

        ~NullProgram() = default;

        bool IsSupported() const override { return false; }
    };

    SPtr<GpuProgram> NullProgramFactory::Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<NullProgram> ret = te_core_ptr<NullProgram>(new (te_allocate<NullProgram>())NullProgram());
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<GpuProgram> NullProgramFactory::Create(GpuProgramType type, GpuDeviceFlags deviceMask)
    {
        SPtr<NullProgram> ret = te_core_ptr<NullProgram>(new (te_allocate<NullProgram>())NullProgram());
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<GpuProgramBytecode> NullProgramFactory::CompileBytecode(const GPU_PROGRAM_DESC& desc)
    {
        auto bytecode = te_shared_ptr_new<GpuProgramBytecode>();
        bytecode->CompilerId = "Null";

        return bytecode;
    }

    GpuProgramManager::GpuProgramManager()
    {
        _nullFactory = te_new<NullProgramFactory>();
        AddFactory(sNullLang, _nullFactory);
    }

    GpuProgramManager::~GpuProgramManager()
    {
        te_delete((NullProgramFactory*)_nullFactory);
    }

    void GpuProgramManager::AddFactory(const String& language, GpuProgramFactory* factory)
    {
        _factories[language] = factory;
    }

    void GpuProgramManager::RemoveFactory(const String& language)
    {
        auto iter = _factories.find(language);
        if (iter != _factories.end())
        {
            _factories.erase(iter);
        }
    }

    GpuProgramFactory* GpuProgramManager::GetFactory(const String& language)
    {
        auto iter = _factories.find(language);
        if (iter == _factories.end())
        {
            iter = _factories.find(sNullLang);
        }

        return iter->second;
    }

    bool GpuProgramManager::IsLanguageSupported(const String& lang)
    {
        auto iter = _factories.find(lang);
        return iter != _factories.end();
    }

    SPtr<GpuProgram> GpuProgramManager::Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<GpuProgram> ret = CreateInternal(desc, deviceMask);
        ret->Initialize();

        return ret;
    }

    SPtr<GpuProgram> GpuProgramManager::CreateInternal(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
    {
        GpuProgramFactory* factory = GetFactory(desc.Language);
        SPtr<GpuProgram> ret = factory->Create(desc, deviceMask);

        return ret;
    }

    SPtr<GpuProgramBytecode> GpuProgramManager::CompileBytecode(const GPU_PROGRAM_DESC& desc)
    {
        GpuProgramFactory* factory = GetFactory(desc.Language);
        return factory->CompileBytecode(desc);
    }
}
