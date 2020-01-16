#include "Shader/TeShader.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    std::atomic<UINT32> Shader::NextShaderId;

    Shader::Shader()
        : _desc(SHADER_DESC())
        , _name("shader")
    {
        UINT32 id = Shader::NextShaderId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many shaders, reached maximum id.");
    }

    Shader::Shader(UINT32 id)
        : _desc(SHADER_DESC())
        , _name("shader")
        , _id(id)
    { }

    Shader::Shader(const SHADER_DESC& desc, const String& name, UINT32 id)
        : _desc(desc)
        , _name(name)
        , _id(id)
    { }

    Shader::~Shader()
    { }

    void Shader::Initialize()
    { }

	HShader Shader::Create(const String& name, const SHADER_DESC& desc)
    {
        SPtr<Shader> shaderPtr = _createPtr(name, desc);
		return static_resource_cast<Shader>(ResourceManager()._createResourceHandle(shaderPtr));
    }

    SPtr<Shader> Shader::CreateEmpty()
    {
        UINT32 id = Shader::NextShaderId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many shaders, reached maximum id.");

        SPtr<Shader> shader = te_core_ptr<Shader>(new (te_allocate<Shader>()) Shader());
		shader->SetThisPtr(shader);

		return shader;
    }

    SPtr<Shader> Shader::_createPtr(const String& name, const SHADER_DESC& desc)
    {
        UINT32 id = Shader::NextShaderId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many shaders, reached maximum id.");

        SPtr<Shader> shader = te_core_ptr<Shader>(new (te_allocate<Shader>()) Shader(desc, name, id));
		shader->SetThisPtr(shader);
		shader->Initialize();

		return shader;
    }
}
