#include "TeShaderImporter.h"
#include "Importer/TeShaderImportOptions.h"
#include "Shader/TeShader.h"
#include "Utility/TeFileStream.h"
#include "TeCoreApplication.h"

#include <iostream>

namespace te
{
    const String ShaderImporter::TypeRaster = "raster";
    const String ShaderImporter::TypeDepth = "depth";
    const String ShaderImporter::TypeStencil = "stencil";
    const String ShaderImporter::TypeBlend = "blend";
    const String ShaderImporter::TypePrograms = "programs";

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
        nlohmann::json jsonDocument;
        FileStream file(filePath);

        if (file.Fail())
        {
            TE_ASSERT_ERROR(false, "Cannot open file: " + filePath, __FILE__, __LINE__);
            return nullptr;
        }

        size_t size = file.Size();

        if (size > std::numeric_limits<UINT32>::max())
        {
            TE_ASSERT_ERROR(false, "File size larger than supported!", __FILE__, __LINE__);
        }

        uint8_t* data = static_cast<uint8_t*>(te_allocate(size+1));
        memset(data, 0, size);
        file.Read(static_cast<char*>((void*)data), static_cast<std::streamsize>(size));
        data[size] = (uint8_t)'\0';

        String dataStr((char*)data);

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(JSON_NOEXCEPTION)
        try 
        {
            jsonDocument = nlohmann::json::parse(dataStr);
            ParserData parsedData = Parse(jsonDocument);
        }
        catch (...)
        {
            TE_ASSERT_ERROR(false, "Can't read shader file " + filePath, __FILE__, __LINE__);
        }
#else
        jsonDocument = nlohmann::json::parse(dataStr);
        ParserData parsedData = Parse(jsonDocument);
#endif
     
        SPtr<Shader> shader = Shader::_createPtr(SHADER_DESC());
        shader->SetName(filePath);

        te_delete(data);

        return shader;
    }

    ShaderImporter::ParserData ShaderImporter::Parse(nlohmann::json& doc)
    {
        ParserData data;

        for (auto it = doc.begin(); it != doc.end(); ++it)
        {
            ParseBlock(it.value(), it.key(), data);
        }

        return data;
    }

    void ShaderImporter::ParseBlock(nlohmann::json& doc, const String& type, ParserData& data)
    {
        if (doc.is_object())
        {
            if (type == TypeRaster)
            {
                ParseRasterBlock(doc, data);
            }
            else if (type == TypeDepth)
            {
                ParseDepthBlock(doc, data);
            }
            else if (type == TypeStencil)
            {
                ParseStencilBlock(doc, data);
            }
            else if (type == TypeBlend)
            {
                ParseBlendBlock(doc, data);
            }
        }

        if (doc.is_array() && type == TypePrograms)
        {
            ParseProgramsBlock(doc, data);
        }
    }

    void ShaderImporter::ParseRasterBlock(nlohmann::json& doc, ParserData& data)
    {
        Raster raster
        {
            doc["fill"].get<String>(),
            doc["cull"].get<String>(),
            doc["scissor"].get<bool>(),
            doc["multisample"].get<bool>(),
            doc["lineaa"].get<bool>()
        };
    }

    void ShaderImporter::ParseDepthBlock(nlohmann::json& doc, ParserData& data)
    {
        Depth depth
        {
            doc["read"].get<bool>(),
            doc["write"].get<bool>(),
            doc["compare"].get<String>(),
            doc["bias"].get<float>(),
            doc["scaledBias"].get<float>(),
            doc["clip"].get<bool>()
        };
    }

    void ShaderImporter::ParseStencilBlock(nlohmann::json& doc, ParserData& data)
    {
        Stencil stencil
        {
            doc["enabled"].get<bool>(),
            doc["reference"].get<int>(),
            doc["readmask"].get<int>(),
            doc["writemask"].get<int>(),
            Stencil::StencilOp(),
            Stencil::StencilOp()
        };

        auto fillStencilOp = [](Stencil::StencilOp& op, nlohmann::json& doc) -> void {
            op = {
                doc["fail"].get<String>(),
                doc["zfail"].get<String>(),
                doc["pass"].get<String>(),
                doc["compare"].get<String>()
            };
        };

        nlohmann::json front = doc["front"];
        nlohmann::json back = doc["back"];

        fillStencilOp(stencil.front, front);
        fillStencilOp(stencil.back, back);
    }

    void ShaderImporter::ParseBlendBlock(nlohmann::json& doc, ParserData& data)
    {

    }

    void ShaderImporter::ParseProgramsBlock(nlohmann::json& doc, ParserData& data)
    {
        for (auto it = doc.begin(); it != doc.end(); ++it)
        {
            ParseProgramBlock(it.value(), data);
        }

        // Check is there is at leat one vertex buffer and one pixel buffer
    }

    void ShaderImporter::ParseProgramBlock(nlohmann::json& doc, ParserData& data)
    {
        String acceptedLanguage = "hlsl";
        
        if (gCoreApplication().GetStartUpDesc().RenderAPI == TE_RENDER_API_MODULE_OPENGL)
        {
            acceptedLanguage = "glsl";
        }

        Program program
        {
            doc["type"].get<String>(),
            doc["language"].get<String>(),
            doc["compiled"].get<bool>(),
            doc["path"].get<String>()
        };

        if (acceptedLanguage == program.language)
        {

        }
    }
}
