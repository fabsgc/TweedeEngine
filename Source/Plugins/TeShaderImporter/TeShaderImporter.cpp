#include "TeShaderImporter.h"
#include "Importer/TeShaderImportOptions.h"
#include "Shader/TeShader.h"

namespace te
{
    ShaderImporter::ShaderImporter()
    {
        _extensions.push_back(u8"shader");
    }

    ShaderImporter::~ShaderImporter()
    { }

    bool ShaderImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), tolower);

        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> ShaderImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<ShaderImportOptions>();
    }

    SPtr<Resource> ShaderImporter::Import(const String& filePath, const SPtr<const ImportOptions> importOptions)
    {
        SPtr<Shader> shader = te_shared_ptr_new<Shader>();

        return shader;
    }
}
