#pragma once

#include "TeShaderImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeSamplerState.h"
#include "Material/TeShader.h"
#include "Json/json.h"

namespace te
{
    class ShaderImporter : public BaseImporter
    {
    public:
        static const String TypeRaster;
        static const String TypeDepth;
        static const String TypeStencil;
        static const String TypeBlend;
        static const String TypePrograms;
        static const String TypeSampler;
        static const String TypeOptions;
        static const String TypeTextures;

        enum Language
        {
            HLSL, GLSL
        };

        struct ParserProgram
        {
            bool IsCompiled = false;
            String code = "";
            Language language = HLSL;
        };

        struct ParserData
        {
            SHADER_DESC ShaderDesc;
            BLEND_STATE_DESC BlendDesc;
            RASTERIZER_STATE_DESC RasterizerDesc;
            DEPTH_STENCIL_STATE_DESC DepthStencilDesc;
            UINT32 stencilRefValue = 0;
            UINT32 seqIdx = 0;

            bool BlendIsDefault = true;
            bool RasterizerIsDefault = true;
            bool DepthStencilIsDefault = true;

            ParserProgram VertexProgram;
            ParserProgram PixelProgram;
            ParserProgram GeometryProgram;
            ParserProgram HullProgram;
            ParserProgram DomainProgram;

            Map<String, SamplerState> Samplers;

            String Name;
        };

        struct Raster
        {
            String fill = "solid";
            String cull = "ccw";
            bool scissor = false;
            bool multisample = false;
            bool lineaa = false;
        };

        struct Depth
        {
            bool read = true;
            bool write = true;
            String compare = "less";
            float bias = 0.0f;
            float scaledBias = 0.0f;
            bool clip = true;
        };

        struct Stencil
        {
            struct StencilOp
            {
                String fail = "keep";
                String zfail = "keep";
                String pass = "keep";
                String compare = "always";
            };

            bool enabled = false;
            int reference = 1;
            int readmask = 0;
            int writemask = 0;
            StencilOp front;
            StencilOp back;
        };

        struct Blend
        {
            struct BlendOp
            {
                String source = "one";
                String dest = "one";
                String op = "add";
            };

            struct Target
            {
                UINT8 index = 1;
                bool enabled = true;
                String writemask = "RGBA";
                BlendOp color;
                BlendOp alpha;
            };

            bool dither = false;
            bool independant = false;
            Vector<Target> targets;
        };

        struct Program
        {
            struct Include
            {
                String type = "sampler";
                String name = "color";
            };

            String type = "vertex";
            String language = "hlsl";
            bool compiled = false;
            String path = "vertex.hlsl";
            String version = "vs_5_0";
            String entry = "main";
            Vector<Include> includes;
        };

        struct Sampler
        {
            String name = "color";
            String addressu = "WRAP";
            String addressv = "WRAP";
            String addressw = "WRAP";
            UINT32 bordercolor = 0;
            String filter = "MIN_MAG_MIP_POINT";
            UINT32 maxanisotropy = 0;
            float maxlod = 0.0f;
            float minlod = 0.0f;
            float miplodbias = 0.0f;
            String compare = "never";
        };

        struct Texture
        {
            String name = "texture";
            String type = "texture2d";
        };

        struct Options
        {
            String name = "name";
            bool separable = true;
            String sort = "backtofront";
            bool transparent = false;
            UINT64 priority = 100000;
        };

    public:
        ShaderImporter();
        virtual ~ShaderImporter();

        /** @copydoc BasicImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc BasicImporter::Import */
        SPtr<Resource> Import(const String& filePath, const SPtr<const ImportOptions> importOptions) override;

        /** @copydoc BasicImporter::CreateImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;

    private:
        ParserData Parse(nlohmann::json& doc);
        void ParseBlock(nlohmann::json& doc, const String& type, ParserData& data);
        void ParseRasterBlock(nlohmann::json& doc, ParserData& data);
        void ParseDepthBlock(nlohmann::json& doc, ParserData& data);
        void ParseStencilBlock(nlohmann::json& doc, ParserData& data);
        void ParseBlendBlock(nlohmann::json& doc, ParserData& data);
        void ParseProgramsBlock(nlohmann::json& doc, ParserData& data);
        void ParseProgramBlock(nlohmann::json& doc, ParserData& data);
        void ParseSamplersBlock(nlohmann::json& doc, ParserData& data);
        void ParseSamplerBlock(nlohmann::json& doc, ParserData& data);
        void ParseTexturesBlock(nlohmann::json& doc, ParserData& data);
        void ParseTextureBlock(nlohmann::json& doc, ParserData& data);
        void ParseOptionsBlock(nlohmann::json& doc, ParserData& data);

    private:
        Vector<String> _extensions;
    };
}
