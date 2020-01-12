#include "Shader/TeShader.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    Shader::Shader()
        : _desc(SHADER_DESC())
    { }
    
    Shader::Shader(const SHADER_DESC& desc)
        : _desc(desc)
    { }

    Shader::~Shader()
    { }

    void Shader::Initialize()
    { }

	HShader Shader::Create(const SHADER_DESC& desc)
    {
        SPtr<Shader> shaderPtr = _createPtr(desc);
		return static_resource_cast<Shader>(ResourceManager()._createResourceHandle(shaderPtr));
    }

    SPtr<Shader> Shader::CreateEmpty()
    {
        SPtr<Shader> shader = te_core_ptr<Shader>(new (te_allocate<Shader>()) Shader());
		shader->SetThisPtr(shader);

		return shader;
    }

    SPtr<Shader> Shader::_createPtr(const SHADER_DESC& desc)
    {
        SPtr<Shader> shader = te_core_ptr<Shader>(new (te_allocate<Shader>()) Shader(desc));
		shader->SetThisPtr(shader);
		shader->Initialize();

		return shader;
    }
}
