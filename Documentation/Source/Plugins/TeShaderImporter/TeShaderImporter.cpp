#include "TeShaderImporter.h"
#include "Importer/TeShaderImportOptions.h"
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
    const String ShaderImporter::TypeSampler = "samplers";
    const String ShaderImporter::TypeOptions = "options";
    const String ShaderImporter::TypeTextures = "textures";

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

        uint8_t* data = static_cast<uint8_t*>(te_allocate(static_cast<UINT32>(size)+1));
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
     
        SPtr<Shader> shader = Shader::_createPtr("shader", SHADER_DESC());
        shader->SetName(filePath);
        //shader->SetId();

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
            else if (type == TypeOptions)
            {
                ParseOptionsBlock(doc, data);
            }
        }

        if (doc.is_array())
        {
            if (type == TypePrograms)
            {
                ParseProgramsBlock(doc, data);
            }
            else if (type == TypeTextures)
            {
                ParseTexturesBlock(doc, data);
            }
            else if (type == TypeSampler)
            {
                ParseSamplersBlock(doc, data);
            }
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
        Blend blend
        {
            doc["dither"].get<bool>(),
            doc["independant"].get<bool>(),
            {}
        };

        auto fillBlendOp = [](Blend::BlendOp& op, nlohmann::json& doc) -> void {
            op = {
                doc["source"].get<String>(),
                doc["dest"].get<String>(),
                doc["op"].get<String>()
            };
        };

        for (auto it = doc["targets"].begin(); it != doc["targets"].end(); ++it)
        {
            Blend::Target target
            {
                it.value()["index"].get<UINT8>(),
                it.value()["enabled"].get<bool>(),
                it.value()["writemask"].get<String>(),
                Blend::BlendOp(),
                Blend::BlendOp()
            };

            fillBlendOp(target.color, it.value()["color"]);
            fillBlendOp(target.alpha, it.value()["alpha"]);

            blend.targets.push_back(target);
        }
    }

    void ShaderImporter::ParseProgramsBlock(nlohmann::json& doc, ParserData& data)
    {
        for (auto it = doc.begin(); it != doc.end(); ++it)
        {
            ParseProgramBlock(it.value(), data);
        }
    }

    void ShaderImporter::ParseProgramBlock(nlohmann::json& doc, ParserData& data)
    {
        // Check is there is exactly only one vertex buffer and only one pixel buffer TODO
        // UINT8 countVertexShader = 0;
        // UINT8 countPixelShader = 0;

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
            doc["path"].get<String>(),
            doc["version"].get<String>(),
            doc["entry"].get<String>(),
            {}
        };

        if (acceptedLanguage == program.language)
        {
            // TODO
        }
    }

    void ShaderImporter::ParseSamplersBlock(nlohmann::json& doc, ParserData& data)
    {
        for (auto it = doc.begin(); it != doc.end(); ++it)
        {
            ParseSamplerBlock(it.value(), data);
        }
    }

    void ShaderImporter::ParseSamplerBlock(nlohmann::json& doc, ParserData& data)
    {
        Sampler sampler
        {
            doc["name"].get<String>(),
            doc["addressu"].get<String>(),
            doc["addressv"].get<String>(),
            doc["addressw"].get<String>(),
            doc["bordercolor"].get<UINT32>(),
            doc["filter"].get<String>(),
            doc["maxanisotropy"].get<UINT32>(),
            doc["maxlod"].get<float>(),
            doc["minlod"].get<float>(),
            doc["miplodbias"].get<float>(),
            doc["compare"].get<String>()
        };
    }

    void ShaderImporter::ParseTexturesBlock(nlohmann::json& doc, ParserData& data)
    {
        for (auto it = doc.begin(); it != doc.end(); ++it)
        {
            ParseTextureBlock(it.value(), data);
        }
    }

    void ShaderImporter::ParseTextureBlock(nlohmann::json& doc, ParserData& data)
    {
        Texture texture
        {
            doc["name"].get<String>(),
            doc["type"].get<String>()
        };
    }

    void ShaderImporter::ParseOptionsBlock(nlohmann::json& doc, ParserData& data)
    {
        Options options
        {
            doc["name"].get<String>(),
            doc["separable"].get<bool>(),
            doc["sort"].get<String>(),
            doc["transparent"].get<bool>(),
            doc["priority"].get<UINT64>()
        };
    }
}
