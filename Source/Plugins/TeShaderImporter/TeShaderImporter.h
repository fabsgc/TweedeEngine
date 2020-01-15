#pragma once

#include "TeShaderImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
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
            ParserProgram ComputeProgram;
        };

        struct Raster
        {
            String fill = "solid";
            String cull = "cw";
            bool scissor = true;
            bool multisample = true;
            bool lineaa = true;
        };

        struct Depth
        {
            bool read = true;
            bool write = true;
            String compare = "less";
            float bias = 0.2f;
            float scaledBias = 0.2f;
            bool clip = true;
        };

        struct Stencil
        {
            struct StencilOp
            {
                String fail = "keep";
                String zfail = "incr";
                String pass = "keep";
                String compare = "always";
            };

            bool enabled = true;
            int reference = 1;
            int readmask = 0;
            int writemask = 0;
            StencilOp front;
            StencilOp back;
        };

        struct Blend
        {

        };

        struct Program
        {
            String type = "vertex";
            String language = "hlsl";
            bool compiled = false;
            String path = "vertex.hlsl";
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

    private:
        Vector<String> _extensions;
    };
}