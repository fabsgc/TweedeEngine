#include "TeImGuiTextEditor.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include <cctype>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wsign-compare" 
#endif

namespace te
{
    // https://en.wikipedia.org/wiki/UTF-8
    // We assume that the char is a standalone character (<128) or a leading byte of an UTF-8 code sequence (non-10xxxxxx code)
    static int UTF8CharLength(ImGuiTextEditor::Char c)
    {
        if ((c & 0xFE) == 0xFC)
            return 6;
        if ((c & 0xFC) == 0xF8)
            return 5;
        if ((c & 0xF8) == 0xF0)
            return 4;
        else if ((c & 0xF0) == 0xE0)
            return 3;
        else if ((c & 0xE0) == 0xC0)
            return 2;
        return 1;
    }

    // "Borrowed" from ImGui source
    static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
    {
        if (c < 0x80)
        {
            buf[0] = (char)c;
            return 1;
        }
        if (c < 0x800)
        {
            if (buf_size < 2) return 0;
            buf[0] = (char)(0xc0 + (c >> 6));
            buf[1] = (char)(0x80 + (c & 0x3f));
            return 2;
        }
        if (c >= 0xdc00 && c < 0xe000)
        {
            return 0;
        }
        if (c >= 0xd800 && c < 0xdc00)
        {
            if (buf_size < 4) return 0;
            buf[0] = (char)(0xf0 + (c >> 18));
            buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
            buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
            buf[3] = (char)(0x80 + ((c) & 0x3f));
            return 4;
        }
        //else if (c < 0x10000)
        {
            if (buf_size < 3) return 0;
            buf[0] = (char)(0xe0 + (c >> 12));
            buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
            buf[2] = (char)(0x80 + ((c) & 0x3f));
            return 3;
        }
    }

    static bool TokenizeCStyleString(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        const char* p = in_begin;

        if (*p == '"')
        {
            p++;

            while (p < in_end)
            {
                // handle end of string
                if (*p == '"')
                {
                    out_begin = in_begin;
                    out_end = p + 1;
                    return true;
                }

                // handle escape character for "
                if (*p == '\\' && p + 1 < in_end && p[1] == '"')
                    p++;

                p++;
            }
        }

        return false;
    }

    static bool TokenizeCStyleCharacterLiteral(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        const char* p = in_begin;

        if (*p == '\'')
        {
            p++;

            // handle escape characters
            if (p < in_end && *p == '\\')
                p++;

            if (p < in_end)
                p++;

            // handle end of character literal
            if (p < in_end && *p == '\'')
            {
                out_begin = in_begin;
                out_end = p + 1;
                return true;
            }
        }

        return false;
    }

    static bool TokenizeCStyleIdentifier(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        const char* p = in_begin;

        if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
        {
            p++;

            while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_'))
                p++;

            out_begin = in_begin;
            out_end = p;
            return true;
        }

        return false;
    }

    static bool TokenizeCStyleNumber(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        const char* p = in_begin;

        const bool startsWithNumber = *p >= '0' && *p <= '9';

        if (*p != '+' && *p != '-' && !startsWithNumber)
            return false;

        p++;

        bool hasNumber = startsWithNumber;

        while (p < in_end && (*p >= '0' && *p <= '9'))
        {
            hasNumber = true;

            p++;
        }

        if (hasNumber == false)
            return false;

        bool isFloat = false;
        bool isHex = false;
        bool isBinary = false;

        if (p < in_end)
        {
            if (*p == '.')
            {
                isFloat = true;

                p++;

                while (p < in_end && (*p >= '0' && *p <= '9'))
                    p++;
            }
            else if (*p == 'x' || *p == 'X')
            {
                // hex formatted integer of the type 0xef80

                isHex = true;

                p++;

                while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
                    p++;
            }
            else if (*p == 'b' || *p == 'B')
            {
                // binary formatted integer of the type 0b01011101

                isBinary = true;

                p++;

                while (p < in_end && (*p >= '0' && *p <= '1'))
                    p++;
            }
        }

        if (isHex == false && isBinary == false)
        {
            // floating point exponent
            if (p < in_end && (*p == 'e' || *p == 'E'))
            {
                isFloat = true;

                p++;

                if (p < in_end && (*p == '+' || *p == '-'))
                    p++;

                bool hasDigits = false;

                while (p < in_end && (*p >= '0' && *p <= '9'))
                {
                    hasDigits = true;

                    p++;
                }

                if (hasDigits == false)
                    return false;
            }

            // single precision floating point type
            if (p < in_end && *p == 'f')
                p++;
        }

        if (isFloat == false)
        {
            // integer size type
            while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L'))
                p++;
        }

        out_begin = in_begin;
        out_end = p;
        return true;
    }

    static bool TokenizeCStylePunctuation(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        (void)in_end;

        switch (*in_begin)
        {
        case '[':
        case ']':
        case '{':
        case '}':
        case '!':
        case '%':
        case '^':
        case '&':
        case '*':
        case '(':
        case ')':
        case '-':
        case '+':
        case '=':
        case '~':
        case '|':
        case '<':
        case '>':
        case '?':
        case ':':
        case '/':
        case ';':
        case ',':
        case '.':
            out_begin = in_begin;
            out_end = in_begin + 1;
            return true;
        }

        return false;
    }

    static bool IsUTFSequence(char c)
    {
        return (c & 0xC0) == 0x80;
    }

    template<class InputIt1, class InputIt2, class BinaryPredicate>
    bool Equals(InputIt1 first1, InputIt1 last1,
        InputIt2 first2, InputIt2 last2, BinaryPredicate p)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!p(*first1, *first2))
                return false;
        }
        return first1 == last1 && first2 == last2;
    }


    const ImGuiTextEditor::LanguageDefinition& ImGuiTextEditor::LanguageDefinition::CPlusPlus()
    {
        static bool inited = false;
        static LanguageDefinition langDef;
        if (!inited)
        {
            static const char* const cppKeywords[] = {
                "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char16_t", "char32_t", "class",
                "compl", "concept", "const", "constexpr", "const_cast", "continue", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
                "for", "friend", "goto", "if", "import", "inline", "int", "long", "module", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
                "register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this", "thread_local",
                "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
            };
            for (auto& k : cppKeywords)
                langDef._keywords.insert(k);

            static const char* const identifiers[] = {
                "abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
                "ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "printf", "sprintf", "snprintf", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper",
                "std", "string", "vector", "map", "unordered_map", "set", "unordered_set", "min", "max"
            };
            for (auto& k : identifiers)
            {
                Identifier id;
                id.Declaration = "Built-in function";
                langDef._identifiers.insert(std::make_pair(String(k), id));
            }

            langDef._tokenize = [](const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, PaletteIndex& paletteIndex) -> bool
            {
                paletteIndex = PaletteIndex::Max;

                while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin))
                    in_begin++;

                if (in_begin == in_end)
                {
                    out_begin = in_end;
                    out_end = in_end;
                    paletteIndex = PaletteIndex::Default;
                }
                else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
                    paletteIndex = PaletteIndex::String;
                else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
                    paletteIndex = PaletteIndex::CharLiteral;
                else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
                    paletteIndex = PaletteIndex::Identifier;
                else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
                    paletteIndex = PaletteIndex::Number;
                else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
                    paletteIndex = PaletteIndex::Punctuation;

                return paletteIndex != PaletteIndex::Max;
            };

            langDef._commentStart = "/*";
            langDef._commentEnd = "*/";
            langDef._singleLineComment = "//";

            langDef._caseSensitive = true;
            langDef._autoIndentation = true;

            langDef._name = "C++";

            inited = true;
        }
        return langDef;
    }

    const ImGuiTextEditor::LanguageDefinition& ImGuiTextEditor::LanguageDefinition::HLSL()
    {
        static bool inited = false;
        static LanguageDefinition langDef;
        if (!inited)
        {
            static const char* const keywords[] = {
                "AppendStructuredBuffer", "asm", "asm_fragment", "BlendState", "bool", "break", "Buffer", "ByteAddressBuffer", "case", "cbuffer", "centroid", "class", "column_major", "compile", "compile_fragment",
                "CompileShader", "const", "continue", "ComputeShader", "ConsumeStructuredBuffer", "default", "DepthStencilState", "DepthStencilView", "discard", "do", "double", "DomainShader", "dword", "else",
                "export", "extern", "false", "float", "for", "fxgroup", "GeometryShader", "groupshared", "half", "Hullshader", "if", "in", "inline", "inout", "InputPatch", "int", "interface", "line", "lineadj",
                "linear", "LineStream", "matrix", "min16float", "min10float", "min16int", "min12int", "min16uint", "namespace", "nointerpolation", "noperspective", "NULL", "out", "OutputPatch", "packoffset",
                "pass", "pixelfragment", "PixelShader", "point", "PointStream", "precise", "RasterizerState", "RenderTargetView", "return", "register", "row_major", "RWBuffer", "RWByteAddressBuffer", "RWStructuredBuffer",
                "RWTexture1D", "RWTexture1DArray", "RWTexture2D", "RWTexture2DArray", "RWTexture3D", "sample", "sampler", "SamplerState", "SamplerComparisonState", "shared", "snorm", "stateblock", "stateblock_state",
                "static", "string", "struct", "switch", "StructuredBuffer", "tbuffer", "technique", "technique10", "technique11", "texture", "Texture1D", "Texture1DArray", "Texture2D", "Texture2DArray", "Texture2DMS",
                "Texture2DMSArray", "Texture3D", "TextureCube", "TextureCubeArray", "true", "typedef", "triangle", "triangleadj", "TriangleStream", "uint", "uniform", "unorm", "unsigned", "vector", "vertexfragment",
                "VertexShader", "void", "volatile", "while",
                "bool1","bool2","bool3","bool4","double1","double2","double3","double4", "float1", "float2", "float3", "float4", "int1", "int2", "int3", "int4", "in", "out", "inout",
                "uint1", "uint2", "uint3", "uint4", "dword1", "dword2", "dword3", "dword4", "half1", "half2", "half3", "half4",
                "float1x1","float2x1","float3x1","float4x1","float1x2","float2x2","float3x2","float4x2",
                "float1x3","float2x3","float3x3","float4x3","float1x4","float2x4","float3x4","float4x4",
                "half1x1","half2x1","half3x1","half4x1","half1x2","half2x2","half3x2","half4x2",
                "half1x3","half2x3","half3x3","half4x3","half1x4","half2x4","half3x4","half4x4",
            };
            for (auto& k : keywords)
                langDef._keywords.insert(k);

            static const char* const identifiers[] = {
                "abort", "abs", "acos", "all", "AllMemoryBarrier", "AllMemoryBarrierWithGroupSync", "any", "asdouble", "asfloat", "asin", "asint", "asint", "asuint",
                "asuint", "atan", "atan2", "ceil", "CheckAccessFullyMapped", "clamp", "clip", "cos", "cosh", "countbits", "cross", "D3DCOLORtoUBYTE4", "ddx",
                "ddx_coarse", "ddx_fine", "ddy", "ddy_coarse", "ddy_fine", "degrees", "determinant", "DeviceMemoryBarrier", "DeviceMemoryBarrierWithGroupSync",
                "distance", "dot", "dst", "errorf", "EvaluateAttributeAtCentroid", "EvaluateAttributeAtSample", "EvaluateAttributeSnapped", "exp", "exp2",
                "f16tof32", "f32tof16", "faceforward", "firstbithigh", "firstbitlow", "floor", "fma", "fmod", "frac", "frexp", "fwidth", "GetRenderTargetSampleCount",
                "GetRenderTargetSamplePosition", "GroupMemoryBarrier", "GroupMemoryBarrierWithGroupSync", "InterlockedAdd", "InterlockedAnd", "InterlockedCompareExchange",
                "InterlockedCompareStore", "InterlockedExchange", "InterlockedMax", "InterlockedMin", "InterlockedOr", "InterlockedXor", "isfinite", "isinf", "isnan",
                "ldexp", "length", "lerp", "lit", "log", "log10", "log2", "mad", "max", "min", "modf", "msad4", "mul", "noise", "normalize", "pow", "printf",
                "Process2DQuadTessFactorsAvg", "Process2DQuadTessFactorsMax", "Process2DQuadTessFactorsMin", "ProcessIsolineTessFactors", "ProcessQuadTessFactorsAvg",
                "ProcessQuadTessFactorsMax", "ProcessQuadTessFactorsMin", "ProcessTriTessFactorsAvg", "ProcessTriTessFactorsMax", "ProcessTriTessFactorsMin",
                "radians", "rcp", "reflect", "refract", "reversebits", "round", "rsqrt", "saturate", "sign", "sin", "sincos", "sinh", "smoothstep", "sqrt", "step",
                "tan", "tanh", "tex1D", "tex1D", "tex1Dbias", "tex1Dgrad", "tex1Dlod", "tex1Dproj", "tex2D", "tex2D", "tex2Dbias", "tex2Dgrad", "tex2Dlod", "tex2Dproj",
                "tex3D", "tex3D", "tex3Dbias", "tex3Dgrad", "tex3Dlod", "tex3Dproj", "texCUBE", "texCUBE", "texCUBEbias", "texCUBEgrad", "texCUBElod", "texCUBEproj", "transpose", "trunc"
            };
            for (auto& k : identifiers)
            {
                Identifier id;
                id.Declaration = "Built-in function";
                langDef._identifiers.insert(std::make_pair(String(k), id));
            }

            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

            langDef._commentStart = "/*";
            langDef._commentEnd = "*/";
            langDef._singleLineComment = "//";

            langDef._caseSensitive = true;
            langDef._autoIndentation = true;

            langDef._name = "HLSL";

            inited = true;
        }
        return langDef;
    }

    const ImGuiTextEditor::LanguageDefinition& ImGuiTextEditor::LanguageDefinition::GLSL()
    {
        static bool inited = false;
        static LanguageDefinition langDef;
        if (!inited)
        {
            static const char* const keywords[] = {
                "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short",
                "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
                "_Noreturn", "_Static_assert", "_Thread_local"
            };
            for (auto& k : keywords)
                langDef._keywords.insert(k);

            static const char* const identifiers[] = {
                "abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
                "ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper"
            };
            for (auto& k : identifiers)
            {
                Identifier id;
                id.Declaration = "Built-in function";
                langDef._identifiers.insert(std::make_pair(String(k), id));
            }

            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
            langDef._tokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

            langDef._commentStart = "/*";
            langDef._commentEnd = "*/";
            langDef._singleLineComment = "//";

            langDef._caseSensitive = true;
            langDef._autoIndentation = true;

            langDef._name = "GLSL";

            inited = true;
        }
        return langDef;
    }

    ImGuiTextEditor::ImGuiTextEditor()
        : _lineSpacing(1.0f)
        , _undoIndex(0)
        , _tabSize(4)
        , _overwrite(false)
        , _readOnly(false)
        , _withinRender(false)
        , _scrollToCursor(false)
        , _scrollToTop(false)
        , _textChanged(false)
        , _colorizerEnabled(true)
        , _textStart(20.0f)
        , _leftMargin(10)
        , _cursorPositionChanged(false)
        , _colorRangeMin(0)
        , _colorRangeMax(0)
        , _selectionMode(SelectionMode::Normal)
        , _handleKeyboardInputs(true)
        , _handleMouseInputs(true)
        , _ignoreImGuiChild(false)
        , _showWhitespaces(true)
        , _checkComments(true)
        , _startTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
        , _lastClick(-1.0f)
    {
        SetPalette(GetDarkPalette());
        SetLanguageDefinition(LanguageDefinition::CPlusPlus());
        _lines.push_back(Line());
    }

    void ImGuiTextEditor::SetLanguageDefinition(const LanguageDefinition& languageDef)
    {
        _languageDefinition = languageDef;
        _regexList.clear();

        for (auto& r : _languageDefinition._tokenRegexStrings)
            _regexList.push_back(std::make_pair(std::regex(r.first, std::regex_constants::optimize), r.second));

        Colorize();
    }

    void ImGuiTextEditor::SetPalette(const Palette& value)
    {
        _paletteBase = value;
    }

    String ImGuiTextEditor::GetSelectedText() const
    {
        return GetText(_state.SelectionStart, _state.SelectionEnd);
    }

    String ImGuiTextEditor::GetCurrentLineText()const
    {
        auto lineLength = GetLineMaxColumn(_state.CursorPosition.Line);
        return GetText(
            Coordinates(_state.CursorPosition.Line, 0),
            Coordinates(_state.CursorPosition.Line, lineLength));
    }

    void ImGuiTextEditor::SetReadOnly(bool value)
    {
        _readOnly = value;
    }

    void ImGuiTextEditor::SetColorizerEnable(bool value)
    {
        _colorizerEnabled = value;
    }

    void ImGuiTextEditor::SetCursorPosition(const Coordinates& position)
    {
        if (_state.CursorPosition != position)
        {
            _state.CursorPosition = position;
            _cursorPositionChanged = true;
            EnsureCursorVisible();
        }
    }

    void ImGuiTextEditor::SetTabSize(int value)
    {
        _tabSize = std::max(0, std::min(32, value));
    }

    void ImGuiTextEditor::InsertText(const String& value)
    {
        InsertText(value.c_str());
    }

    void ImGuiTextEditor::InsertText(const char* value)
    {
        if (value == nullptr)
            return;

        auto pos = GetActualCursorCoordinates();
        auto start = std::min(pos, _state.SelectionStart);
        int totalLines = pos.Line - start.Line;

        totalLines += InsertTextAt(pos, value);

        SetSelection(pos, pos);
        SetCursorPosition(pos);
        Colorize(start.Line - 1, totalLines + 2);
    }

    void ImGuiTextEditor::MoveUp(int amount, bool select)
    {
        auto oldPos = _state.CursorPosition;
        _state.CursorPosition.Line = std::max(0, _state.CursorPosition.Line - amount);
        if (oldPos != _state.CursorPosition)
        {
            if (select)
            {
                if (oldPos == _interactiveStart)
                    _interactiveStart = _state.CursorPosition;
                else if (oldPos == _interactiveEnd)
                    _interactiveEnd = _state.CursorPosition;
                else
                {
                    _interactiveStart = _state.CursorPosition;
                    _interactiveEnd = oldPos;
                }
            }
            else
                _interactiveStart = _interactiveEnd = _state.CursorPosition;
            SetSelection(_interactiveStart, _interactiveEnd);

            EnsureCursorVisible();
        }
    }

    void ImGuiTextEditor::MoveDown(int amount, bool select)
    {
        assert(_state.CursorPosition.Column >= 0);
        auto oldPos = _state.CursorPosition;
        _state.CursorPosition.Line = std::max(0, std::min((int)_lines.size() - 1, _state.CursorPosition.Line + amount));

        if (_state.CursorPosition != oldPos)
        {
            if (select)
            {
                if (oldPos == _interactiveEnd)
                    _interactiveEnd = _state.CursorPosition;
                else if (oldPos == _interactiveStart)
                    _interactiveStart = _state.CursorPosition;
                else
                {
                    _interactiveStart = oldPos;
                    _interactiveEnd = _state.CursorPosition;
                }
            }
            else
                _interactiveStart = _interactiveEnd = _state.CursorPosition;
            SetSelection(_interactiveStart, _interactiveEnd);

            EnsureCursorVisible();
        }
    }

    void ImGuiTextEditor::MoveLeft(int amount, bool select, bool wordMode)
    {
        if (_lines.empty())
            return;

        auto oldPos = _state.CursorPosition;
        _state.CursorPosition = GetActualCursorCoordinates();
        auto line = _state.CursorPosition.Line;
        auto cindex = GetCharacterIndex(_state.CursorPosition);

        while (amount-- > 0)
        {
            if (cindex == 0)
            {
                if (line > 0)
                {
                    --line;
                    if ((int)_lines.size() > line)
                        cindex = (int)_lines[line].size();
                    else
                        cindex = 0;
                }
            }
            else
            {
                --cindex;
                if (cindex > 0)
                {
                    if ((int)_lines.size() > line)
                    {
                        while (cindex > 0 && IsUTFSequence(_lines[line][cindex].Character))
                            --cindex;
                    }
                }
            }

            _state.CursorPosition = Coordinates(line, GetCharacterColumn(line, cindex));
            if (wordMode)
            {
                _state.CursorPosition = FindWordStart(_state.CursorPosition);
                cindex = GetCharacterIndex(_state.CursorPosition);
            }
        }

        _state.CursorPosition = Coordinates(line, GetCharacterColumn(line, cindex));

        assert(_state.CursorPosition.Column >= 0);
        if (select)
        {
            if (oldPos == _interactiveStart)
                _interactiveStart = _state.CursorPosition;
            else if (oldPos == _interactiveEnd)
                _interactiveEnd = _state.CursorPosition;
            else
            {
                _interactiveStart = _state.CursorPosition;
                _interactiveEnd = oldPos;
            }
        }
        else
            _interactiveStart = _interactiveEnd = _state.CursorPosition;
        SetSelection(_interactiveStart, _interactiveEnd, select && wordMode ? SelectionMode::Word : SelectionMode::Normal);

        EnsureCursorVisible();
    }

    void ImGuiTextEditor::MoveRight(int amount, bool select, bool wordMode)
    {
        auto oldPos = _state.CursorPosition;

        if (_lines.empty() || (size_t)(oldPos.Line) >= _lines.size())
            return;

        auto cindex = GetCharacterIndex(_state.CursorPosition);
        while (amount-- > 0)
        {
            auto lindex = _state.CursorPosition.Line;
            auto& line = _lines[lindex];

            if ((size_t)cindex >= line.size())
            {
                if ((size_t)(_state.CursorPosition.Line) < _lines.size() - 1)
                {
                    _state.CursorPosition.Line = std::max(0, std::min((int)_lines.size() - 1, _state.CursorPosition.Line + 1));
                    _state.CursorPosition.Column = 0;
                }
                else
                    return;
            }
            else
            {
                cindex += UTF8CharLength(line[cindex].Character);
                _state.CursorPosition = Coordinates(lindex, GetCharacterColumn(lindex, cindex));
                if (wordMode)
                    _state.CursorPosition = FindNextWord(_state.CursorPosition);
            }
        }

        if (select)
        {
            if (oldPos == _interactiveEnd)
                _interactiveEnd = SanitizeCoordinates(_state.CursorPosition);
            else if (oldPos == _interactiveStart)
                _interactiveStart = _state.CursorPosition;
            else
            {
                _interactiveStart = oldPos;
                _interactiveEnd = _state.CursorPosition;
            }
        }
        else
            _interactiveStart = _interactiveEnd = _state.CursorPosition;
        SetSelection(_interactiveStart, _interactiveEnd, select && wordMode ? SelectionMode::Word : SelectionMode::Normal);

        EnsureCursorVisible();
    }

    void ImGuiTextEditor::MoveTop(bool select)
    {
        auto oldPos = _state.CursorPosition;
        SetCursorPosition(Coordinates(0, 0));

        if (_state.CursorPosition != oldPos)
        {
            if (select)
            {
                _interactiveEnd = oldPos;
                _interactiveStart = _state.CursorPosition;
            }
            else
                _interactiveStart = _interactiveEnd = _state.CursorPosition;
            SetSelection(_interactiveStart, _interactiveEnd);
        }
    }

    void ImGuiTextEditor::MoveBottom(bool select)
    {
        auto oldPos = GetCursorPosition();
        auto newPos = Coordinates((int)_lines.size() - 1, 0);
        SetCursorPosition(newPos);
        if (select)
        {
            _interactiveStart = oldPos;
            _interactiveEnd = newPos;
        }
        else
            _interactiveStart = _interactiveEnd = newPos;
        SetSelection(_interactiveStart, _interactiveEnd);
    }

    void ImGuiTextEditor::MoveHome(bool select)
    {
        auto oldPos = _state.CursorPosition;
        SetCursorPosition(Coordinates(_state.CursorPosition.Line, 0));

        if (_state.CursorPosition != oldPos)
        {
            if (select)
            {
                if (oldPos == _interactiveStart)
                    _interactiveStart = _state.CursorPosition;
                else if (oldPos == _interactiveEnd)
                    _interactiveEnd = _state.CursorPosition;
                else
                {
                    _interactiveStart = _state.CursorPosition;
                    _interactiveEnd = oldPos;
                }
            }
            else
                _interactiveStart = _interactiveEnd = _state.CursorPosition;
            SetSelection(_interactiveStart, _interactiveEnd);
        }
    }

    void ImGuiTextEditor::MoveEnd(bool select)
    {
        auto oldPos = _state.CursorPosition;
        SetCursorPosition(Coordinates(_state.CursorPosition.Line, GetLineMaxColumn(oldPos.Line)));

        if (_state.CursorPosition != oldPos)
        {
            if (select)
            {
                if (oldPos == _interactiveEnd)
                    _interactiveEnd = _state.CursorPosition;
                else if (oldPos == _interactiveStart)
                    _interactiveStart = _state.CursorPosition;
                else
                {
                    _interactiveStart = oldPos;
                    _interactiveEnd = _state.CursorPosition;
                }
            }
            else
                _interactiveStart = _interactiveEnd = _state.CursorPosition;
            SetSelection(_interactiveStart, _interactiveEnd);
        }
    }

    void ImGuiTextEditor::SetSelectionStart(const Coordinates& position)
    {
        _state.SelectionStart = SanitizeCoordinates(position);
        if (_state.SelectionStart > _state.SelectionEnd)
            std::swap(_state.SelectionStart, _state.SelectionEnd);
    }

    void ImGuiTextEditor::SetSelectionEnd(const Coordinates& position)
    {
        _state.SelectionEnd = SanitizeCoordinates(position);
        if (_state.SelectionStart > _state.SelectionEnd)
            std::swap(_state.SelectionStart, _state.SelectionEnd);
    }

    void ImGuiTextEditor::SetSelection(const Coordinates& start, const Coordinates& end, SelectionMode mode)
    {
        auto oldSelStart = _state.SelectionStart;
        auto oldSelEnd = _state.SelectionEnd;

        _state.SelectionStart = SanitizeCoordinates(start);
        _state.SelectionEnd = SanitizeCoordinates(end);
        if (_state.SelectionStart > _state.SelectionEnd)
            std::swap(_state.SelectionStart, _state.SelectionEnd);

        switch (mode)
        {
        case ImGuiTextEditor::SelectionMode::Normal:
            break;
        case ImGuiTextEditor::SelectionMode::Word:
        {
            _state.SelectionStart = FindWordStart(_state.SelectionStart);
            if (!IsOnWordBoundary(_state.SelectionEnd))
                _state.SelectionEnd = FindWordEnd(FindWordStart(_state.SelectionEnd));
            break;
        }
        case ImGuiTextEditor::SelectionMode::Line:
        {
            const auto lineNo = _state.SelectionEnd.Line;
            //const auto lineSize = (size_t)lineNo < _lines.size() ? _lines[lineNo].size() : 0;
            _state.SelectionStart = Coordinates(_state.SelectionStart.Line, 0);
            _state.SelectionEnd = Coordinates(lineNo, GetLineMaxColumn(lineNo));
            break;
        }
        default:
            break;
        }

        if (_state.SelectionStart != oldSelStart ||
            _state.SelectionEnd != oldSelEnd)
            _cursorPositionChanged = true;
    }

    void ImGuiTextEditor::SelectWordUnderCursor()
    {
        auto c = GetCursorPosition();
        SetSelection(FindWordStart(c), FindWordEnd(c));
    }

    void ImGuiTextEditor::SelectAll()
    {
        SetSelection(Coordinates(0, 0), Coordinates((int)_lines.size(), 0));
    }

    bool ImGuiTextEditor::HasSelection() const
    {
        return _state.SelectionEnd > _state.SelectionStart;
    }

    void ImGuiTextEditor::Copy()
    {
        if (HasSelection())
        {
            ImGui::SetClipboardText(GetSelectedText().c_str());
        }
        else
        {
            if (!_lines.empty())
            {
                String str;
                auto& line = _lines[GetActualCursorCoordinates().Line];
                for (auto& g : line)
                    str.push_back(g.Character);
                ImGui::SetClipboardText(str.c_str());
            }
        }
    }

    void ImGuiTextEditor::Cut()
    {
        if (IsReadOnly())
        {
            Copy();
        }
        else
        {
            if (HasSelection())
            {
                UndoRecord u;
                u._before = _state;
                u._removed = GetSelectedText();
                u._removedStart = _state.SelectionStart;
                u._removedEnd = _state.SelectionEnd;

                Copy();
                DeleteSelection();

                u._after = _state;
                AddUndo(u);
            }
        }
    }

    void ImGuiTextEditor::Paste()
    {
        if (IsReadOnly())
            return;

        auto clipText = ImGui::GetClipboardText();
        if (clipText != nullptr && strlen(clipText) > 0)
        {
            UndoRecord u;
            u._before = _state;

            if (HasSelection())
            {
                u._removed = GetSelectedText();
                u._removedStart = _state.SelectionStart;
                u._removedEnd = _state.SelectionEnd;
                DeleteSelection();
            }

            u._added = clipText;
            u._addedStart = GetActualCursorCoordinates();

            InsertText(clipText);

            u._addedEnd = GetActualCursorCoordinates();
            u._after = _state;
            AddUndo(u);
        }
    }

    void ImGuiTextEditor::Delete()
    {
        assert(!_readOnly);

        if (_lines.empty())
            return;

        UndoRecord u;
        u._before = _state;

        if (HasSelection())
        {
            u._removed = GetSelectedText();
            u._removedStart = _state.SelectionStart;
            u._removedEnd = _state.SelectionEnd;

            DeleteSelection();
        }
        else
        {
            auto pos = GetActualCursorCoordinates();
            SetCursorPosition(pos);
            auto& line = _lines[pos.Line];

            if (pos.Column == GetLineMaxColumn(pos.Line))
            {
                if (pos.Line == (int)_lines.size() - 1)
                    return;

                u._removed = '\n';
                u._removedStart = u._removedEnd = GetActualCursorCoordinates();
                Advance(u._removedEnd);

                auto& nextLine = _lines[pos.Line + 1];
                line.insert(line.end(), nextLine.begin(), nextLine.end());
                RemoveLine(pos.Line + 1);
            }
            else
            {
                auto cindex = GetCharacterIndex(pos);
                u._removedStart = u._removedEnd = GetActualCursorCoordinates();
                u._removedEnd.Column++;
                u._removed = GetText(u._removedStart, u._removedEnd);

                auto d = UTF8CharLength(line[cindex].Character);
                while (d-- > 0 && cindex < (int)line.size())
                    line.erase(line.begin() + cindex);
            }

            _textChanged = true;

            Colorize(pos.Line, 1);
        }

        u._after = _state;
        AddUndo(u);
    }

    bool ImGuiTextEditor::CanUndo() const
    {
        return !_readOnly && _undoIndex > 0;
    }

    bool ImGuiTextEditor::CanRedo() const
    {
        return !_readOnly && _undoIndex < (int)_undoBuffer.size();
    }

    void ImGuiTextEditor::Undo(int steps)
    {
        while (CanUndo() && steps-- > 0)
            _undoBuffer[--_undoIndex].Undo(this);
    }

    void ImGuiTextEditor::Redo(int steps)
    {
        while (CanRedo() && steps-- > 0)
            _undoBuffer[_undoIndex++].Redo(this);
    }

    const ImGuiTextEditor::Palette& ImGuiTextEditor::GetDarkPalette()
    {
        const static Palette p = { {
                0xff7f7f7f,    // Default
                0xffd69c56,    // Keyword    
                0xff00ff00,    // Number
                0xff7070e0,    // String
                0xff70a0e0, // Char literal
                0xffffffff, // Punctuation
                0xff408080,    // Preprocessor
                0xffaaaaaa, // Identifier
                0xff9bc64d, // Known identifier
                0xffc040a0, // Preproc identifier
                0xff206020, // Comment (single line)
                0xff406020, // Comment (multi line)
                0xff101010, // Background
                0xffe0e0e0, // Cursor
                0x80a06020, // Selection
                0x800020ff, // ErrorMarker
                0x40f08000, // Breakpoint
                0xff707000, // Line number
                0x40000000, // Current line fill
                0x40808080, // Current line fill (inactive)
                0x40a0a0a0, // Current line edge
            } };
        return p;
    }

    const ImGuiTextEditor::Palette& ImGuiTextEditor::GetLightPalette()
    {
        const static Palette p = { {
                0xff7f7f7f, // None
                0xffff0c06, // Keyword    
                0xff008000, // Number
                0xff2020a0, // String
                0xff304070, // Char literal
                0xff000000, // Punctuation
                0xff406060, // Preprocessor
                0xff404040, // Identifier
                0xff606010, // Known identifier
                0xffc040a0, // Preproc identifier
                0xff205020, // Comment (single line)
                0xff405020, // Comment (multi line)
                0xffffffff, // Background
                0xff000000, // Cursor
                0x80600000, // Selection
                0xa00010ff, // ErrorMarker
                0x80f08000, // Breakpoint
                0xff505000, // Line number
                0x40000000, // Current line fill
                0x40808080, // Current line fill (inactive)
                0x40000000, // Current line edge
            } };
        return p;
    }

    const ImGuiTextEditor::Palette& ImGuiTextEditor::GetRetroBluePalette()
    {
        const static Palette p = { {
                0xff00ffff, // None
                0xffffff00, // Keyword    
                0xff00ff00, // Number
                0xff808000, // String
                0xff808000, // Char literal
                0xffffffff, // Punctuation
                0xff008000, // Preprocessor
                0xff00ffff, // Identifier
                0xffffffff, // Known identifier
                0xffff00ff, // Preproc identifier
                0xff808080, // Comment (single line)
                0xff404040, // Comment (multi line)
                0xff800000, // Background
                0xff0080ff, // Cursor
                0x80ffff00, // Selection
                0xa00000ff, // ErrorMarker
                0x80ff8000, // Breakpoint
                0xff808000, // Line number
                0x40000000, // Current line fill
                0x40808080, // Current line fill (inactive)
                0x40000000, // Current line edge
            } };
        return p;
    }

    ImGuiTextEditor::UndoRecord::UndoRecord(
        const String& added,
        const ImGuiTextEditor::Coordinates addedStart,
        const ImGuiTextEditor::Coordinates addedEnd,
        const String& removed,
        const ImGuiTextEditor::Coordinates removedStart,
        const ImGuiTextEditor::Coordinates removedEnd,
        ImGuiTextEditor::EditorState& before,
        ImGuiTextEditor::EditorState& after)
        : _added(added)
        , _addedStart(addedStart)
        , _addedEnd(addedEnd)
        , _removed(removed)
        , _removedStart(removedStart)
        , _removedEnd(removedEnd)
        , _before(before)
        , _after(after)
    {
        assert(addedStart <= addedEnd);
        assert(removedStart <= removedEnd);
    }

    void ImGuiTextEditor::UndoRecord::Undo(ImGuiTextEditor* editor)
    {
        if (!_added.empty())
        {
            editor->DeleteRange(_addedStart, _addedEnd);
            editor->Colorize(_addedStart.Line - 1, _addedEnd.Line - _addedStart.Line + 2);
        }

        if (!_removed.empty())
        {
            auto start = _removedStart;
            editor->InsertTextAt(start, _removed.c_str());
            editor->Colorize(_removedStart.Line - 1, _removedEnd.Line - _removedStart.Line + 2);
        }

        editor->_state = _before;
        editor->EnsureCursorVisible();
    }

    void ImGuiTextEditor::UndoRecord::Redo(ImGuiTextEditor* editor)
    {
        if (!_removed.empty())
        {
            editor->DeleteRange(_removedStart, _removedEnd);
            editor->Colorize(_removedStart.Line - 1, _removedEnd.Line - _removedStart.Line + 1);
        }

        if (!_added.empty())
        {
            auto start = _addedStart;
            editor->InsertTextAt(start, _added.c_str());
            editor->Colorize(_addedStart.Line - 1, _addedEnd.Line - _addedStart.Line + 1);
        }

        editor->_state = _after;
        editor->EnsureCursorVisible();
    }

    void ImGuiTextEditor::Render(const char* title, const ImVec2& size, bool border)
    {
        _withinRender = true;
        _textChanged = false;
        _cursorPositionChanged = false;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(_palette[(int)PaletteIndex::Background]));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        if (!_ignoreImGuiChild)
            ImGui::BeginChild(title, size, border, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove);

        if (_handleKeyboardInputs)
        {
            HandleKeyboardInputs();
            ImGui::PushAllowKeyboardFocus(true);
        }

        if (_handleMouseInputs)
            HandleMouseInputs();

        ColorizeInternal();
        Render();

        if (_handleKeyboardInputs)
            ImGui::PopAllowKeyboardFocus();

        if (!_ignoreImGuiChild)
            ImGui::EndChild();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        _withinRender = false;
    }

    void ImGuiTextEditor::SetText(const String& aText)
    {
        _lines.clear();
        _lines.emplace_back(Line());
        for (auto chr : aText)
        {
            if (chr == '\r')
            {
                // ignore the carriage return character
            }
            else if (chr == '\n')
                _lines.emplace_back(Line());
            else
            {
                _lines.back().emplace_back(Glyph(chr, PaletteIndex::Default));
            }
        }

        _textChanged = true;
        _scrollToTop = true;

        _undoBuffer.clear();
        _undoIndex = 0;

        Colorize();
    }

    String ImGuiTextEditor::GetText() const
    {
        return GetText(Coordinates(), Coordinates((int)_lines.size(), 0));
    }

    void ImGuiTextEditor::SetTextLines(const Vector<String>& aLines)
    {
        _lines.clear();

        if (aLines.empty())
        {
            _lines.emplace_back(Line());
        }
        else
        {
            _lines.resize(aLines.size());

            for (size_t i = 0; i < aLines.size(); ++i)
            {
                const String& aLine = aLines[i];

                _lines[i].reserve(aLine.size());
                for (size_t j = 0; j < aLine.size(); ++j)
                    _lines[i].emplace_back(Glyph(aLine[j], PaletteIndex::Default));
            }
        }

        _textChanged = true;
        _scrollToTop = true;

        _undoBuffer.clear();
        _undoIndex = 0;

        Colorize();
    }

    Vector<String> ImGuiTextEditor::GetTextLines() const
    {
        Vector<String> result;

        result.reserve(_lines.size());

        for (auto& line : _lines)
        {
            String text;

            text.resize(line.size());

            for (size_t i = 0; i < line.size(); ++i)
                text[i] = line[i].Character;

            result.emplace_back(std::move(text));
        }

        return result;
    }

    void ImGuiTextEditor::Colorize(int fromLine, int lines)
    {
        int toLine = lines == -1 ? (int)_lines.size() : std::min((int)_lines.size(), fromLine + lines);
        _colorRangeMin = std::min(_colorRangeMin, fromLine);
        _colorRangeMax = std::max(_colorRangeMax, toLine);
        _colorRangeMin = std::max(0, _colorRangeMin);
        _colorRangeMax = std::max(_colorRangeMin, _colorRangeMax);
        _checkComments = true;
    }

    void ImGuiTextEditor::ColorizeRange(int fromLine, int toLine)
    {
        if (_lines.empty() || fromLine >= toLine)
            return;

        String buffer;
        std::cmatch results;
        String id;

        int endLine = std::max(0, std::min((int)_lines.size(), toLine));
        for (int i = fromLine; i < endLine; ++i)
        {
            auto& line = _lines[i];

            if (line.empty())
                continue;

            buffer.resize(line.size());
            for (size_t j = 0; j < line.size(); ++j)
            {
                auto& col = line[j];
                buffer[j] = col.Character;
                col.ColorIndex = PaletteIndex::Default;
            }

            const char* bufferBegin = &buffer.front();
            const char* bufferEnd = bufferBegin + buffer.size();

            auto last = bufferEnd;

            for (auto first = bufferBegin; first != last; )
            {
                const char* token_begin = nullptr;
                const char* token_end = nullptr;
                PaletteIndex token_color = PaletteIndex::Default;

                bool hasTokenizeResult = false;

                if (_languageDefinition._tokenize != nullptr)
                {
                    if (_languageDefinition._tokenize(first, last, token_begin, token_end, token_color))
                        hasTokenizeResult = true;
                }

                if (hasTokenizeResult == false)
                {
                    // todo : remove
                    //printf("using regex for %.*s\n", first + 10 < last ? 10 : int(last - first), first);

                    for (auto& p : _regexList)
                    {
                        if (std::regex_search(first, last, results, p.first, std::regex_constants::match_continuous))
                        {
                            hasTokenizeResult = true;

                            auto& v = *results.begin();
                            token_begin = v.first;
                            token_end = v.second;
                            token_color = p.second;
                            break;
                        }
                    }
                }

                if (hasTokenizeResult == false)
                {
                    first++;
                }
                else
                {
                    const size_t token_length = token_end - token_begin;

                    if (token_color == PaletteIndex::Identifier)
                    {
                        id.assign(token_begin, token_end);

                        // todo : allmost all language definitions use lower case to specify keywords, so shouldn't this use ::tolower ?
                        if (!_languageDefinition._caseSensitive)
                            std::transform(id.begin(), id.end(), id.begin(), 
                                [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::toupper(c)); });

                        if (!line[first - bufferBegin].Preprocessor)
                        {
                            if (_languageDefinition._keywords.count(id) != 0)
                                token_color = PaletteIndex::Keyword;
                            else if (_languageDefinition._identifiers.count(id) != 0)
                                token_color = PaletteIndex::KnownIdentifier;
                            else if (_languageDefinition._preprocIdentifiers.count(id) != 0)
                                token_color = PaletteIndex::PreprocIdentifier;
                        }
                        else
                        {
                            if (_languageDefinition._preprocIdentifiers.count(id) != 0)
                                token_color = PaletteIndex::PreprocIdentifier;
                        }
                    }

                    for (size_t j = 0; j < token_length; ++j)
                        line[(token_begin - bufferBegin) + j].ColorIndex = token_color;

                    first = token_end;
                }
            }
        }
    }

    void ImGuiTextEditor::ColorizeInternal()
    {
        if (_lines.empty() || !_colorizerEnabled)
            return;

        if (_checkComments)
        {
            auto endLine = _lines.size();
            auto endIndex = 0;
            auto commentStartLine = endLine;
            auto commentStartIndex = endIndex;
            auto withinString = false;
            auto withinSingleLineComment = false;
            auto withinPreproc = false;
            auto firstChar = true;            // there is no other non-whitespace characters in the line before
            auto concatenate = false;        // '\' on the very end of the line
            auto currentLine = 0;
            auto currentIndex = 0;
            while ((size_t)currentLine < endLine || currentIndex < endIndex)
            {
                auto& line = _lines[currentLine];

                if (currentIndex == 0 && !concatenate)
                {
                    withinSingleLineComment = false;
                    withinPreproc = false;
                    firstChar = true;
                }

                concatenate = false;

                if (!line.empty())
                {
                    auto& g = line[currentIndex];
                    auto c = g.Character;

                    if (c != _languageDefinition._preprocChar && !isspace(c))
                        firstChar = false;

                    if (currentIndex == (int)line.size() - 1 && line[line.size() - 1].Character == '\\')
                        concatenate = true;

                    bool inComment = (commentStartLine < (size_t)currentLine || (commentStartLine == (size_t)currentLine && commentStartIndex <= currentIndex));

                    if (withinString)
                    {
                        line[currentIndex].MultiLineComment = inComment;

                        if (c == '\"')
                        {
                            if (currentIndex + 1 < (int)line.size() && line[currentIndex + 1].Character == '\"')
                            {
                                currentIndex += 1;
                                if (currentIndex < (int)line.size())
                                    line[currentIndex].MultiLineComment = inComment;
                            }
                            else
                                withinString = false;
                        }
                        else if (c == '\\')
                        {
                            currentIndex += 1;
                            if (currentIndex < (int)line.size())
                                line[currentIndex].MultiLineComment = inComment;
                        }
                    }
                    else
                    {
                        if (firstChar && c == _languageDefinition._preprocChar)
                            withinPreproc = true;

                        if (c == '\"')
                        {
                            withinString = true;
                            line[currentIndex].MultiLineComment = inComment;
                        }
                        else
                        {
                            auto pred = [](const char& a, const Glyph& b) { return a == b.Character; };
                            auto from = line.begin() + currentIndex;
                            auto& startStr = _languageDefinition._commentStart;
                            auto& singleStartStr = _languageDefinition._singleLineComment;

                            if (singleStartStr.size() > 0 &&
                                currentIndex + singleStartStr.size() <= line.size() &&
                                Equals(singleStartStr.begin(), singleStartStr.end(), from, from + singleStartStr.size(), pred))
                            {
                                withinSingleLineComment = true;
                            }
                            else if (!withinSingleLineComment && currentIndex + startStr.size() <= line.size() &&
                                Equals(startStr.begin(), startStr.end(), from, from + startStr.size(), pred))
                            {
                                commentStartLine = currentLine;
                                commentStartIndex = currentIndex;
                            }

                            inComment = (commentStartLine < (size_t)currentLine || (commentStartLine == (size_t)currentLine && commentStartIndex <= currentIndex));

                            line[currentIndex].MultiLineComment = inComment;
                            line[currentIndex].Comment = withinSingleLineComment;

                            auto& endStr = _languageDefinition._commentEnd;
                            if (currentIndex + 1 >= (int)endStr.size() &&
                                Equals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred))
                            {
                                commentStartIndex = endIndex;
                                commentStartLine = endLine;
                            }
                        }
                    }
                    line[currentIndex].Preprocessor = withinPreproc;
                    currentIndex += UTF8CharLength(c);
                    if (currentIndex >= (int)line.size())
                    {
                        currentIndex = 0;
                        ++currentLine;
                    }
                }
                else
                {
                    currentIndex = 0;
                    ++currentLine;
                }
            }
            _checkComments = false;
        }

        if (_colorRangeMin < _colorRangeMax)
        {
            const int increment = (_languageDefinition._tokenize == nullptr) ? 10 : 10000;
            const int to = std::min(_colorRangeMin + increment, _colorRangeMax);
            ColorizeRange(_colorRangeMin, to);
            _colorRangeMin = to;

            if (_colorRangeMax == _colorRangeMin)
            {
                _colorRangeMin = std::numeric_limits<int>::max();
                _colorRangeMax = 0;
            }
            return;
        }
    }

    float ImGuiTextEditor::TextDistanceToLineStart(const Coordinates& from) const
    {
        auto& line = _lines[from.Line];
        float distance = 0.0f;
        float spaceSize = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;
        int colIndex = GetCharacterIndex(from);
        for (size_t it = 0u; it < line.size() && it < (size_t)colIndex; )
        {
            if (line[it].Character == '\t')
            {
                distance = (1.0f + std::floor((1.0f + distance) / (float(_tabSize) * spaceSize))) * (float(_tabSize) * spaceSize);
                ++it;
            }
            else
            {
                auto d = UTF8CharLength(line[it].Character);
                char tempCString[7];
                int i = 0;
                for (; i < 6 && d-- > 0 && it < line.size(); i++, it++)
                    tempCString[i] = line[it].Character;

                tempCString[i] = '\0';
                distance += ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, tempCString, nullptr, nullptr).x;
            }
        }

        return distance;
    }

    void ImGuiTextEditor::EnsureCursorVisible()
    {
        if (!_withinRender)
        {
            _scrollToCursor = true;
            return;
        }

        float scrollX = ImGui::GetScrollX();
        float scrollY = ImGui::GetScrollY();

        auto height = ImGui::GetWindowHeight();
        auto width = ImGui::GetWindowWidth();

        auto top = 1 + (int)ceil(scrollY / _charAdvance.y);
        auto bottom = (int)ceil((scrollY + height) / _charAdvance.y);

        auto left = (int)ceil(scrollX / _charAdvance.x);
        auto right = (int)ceil((scrollX + width) / _charAdvance.x);

        auto pos = GetActualCursorCoordinates();
        auto len = TextDistanceToLineStart(pos);

        if (pos.Line < top)
            ImGui::SetScrollY(std::max(0.0f, (pos.Line - 1) * _charAdvance.y));
        if (pos.Line > bottom - 4)
            ImGui::SetScrollY(std::max(0.0f, (pos.Line + 4) * _charAdvance.y - height));
        if (len + _textStart < left + 4)
            ImGui::SetScrollX(std::max(0.0f, len + _textStart - 4));
        if (len + _textStart > right - 4)
            ImGui::SetScrollX(std::max(0.0f, len + _textStart + 4 - width));
    }

    int ImGuiTextEditor::GetPageSize() const
    {
        auto height = ImGui::GetWindowHeight() - 20.0f;
        return (int)floor(height / _charAdvance.y);
    }

    String ImGuiTextEditor::GetText(const Coordinates& start, const Coordinates& end) const
    {
        String result;

        auto lstart = start.Line;
        auto lend = end.Line;
        auto istart = GetCharacterIndex(start);
        auto iend = GetCharacterIndex(end);
        size_t s = 0;

        for (size_t i = (size_t)lstart; i < (size_t)lend; i++)
            s += _lines[i].size();

        result.reserve(s + s / 8);

        while (istart < iend || lstart < lend)
        {
            if (lstart >= (int)_lines.size())
                break;

            auto& line = _lines[lstart];
            if (istart < (int)line.size())
            {
                result += line[istart].Character;
                istart++;
            }
            else
            {
                istart = 0;
                ++lstart;
                result += '\n';
            }
        }

        return result;
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::GetActualCursorCoordinates() const
    {
        return SanitizeCoordinates(_state.CursorPosition);
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::SanitizeCoordinates(const Coordinates& value) const
    {
        auto line = value.Line;
        auto column = value.Column;
        if (line >= (int)_lines.size())
        {
            if (_lines.empty())
            {
                line = 0;
                column = 0;
            }
            else
            {
                line = (int)_lines.size() - 1;
                column = GetLineMaxColumn(line);
            }
            return Coordinates(line, column);
        }
        else
        {
            column = _lines.empty() ? 0 : std::min(column, GetLineMaxColumn(line));
            return Coordinates(line, column);
        }
    }

    void ImGuiTextEditor::Advance(Coordinates& coordinates) const
    {
        if (coordinates.Line < (int)_lines.size())
        {
            auto& line = _lines[coordinates.Line];
            auto cindex = GetCharacterIndex(coordinates);

            if (cindex + 1 < (int)line.size())
            {
                auto delta = UTF8CharLength(line[cindex].Character);
                cindex = std::min(cindex + delta, (int)line.size() - 1);
            }
            else
            {
                ++coordinates.Line;
                cindex = 0;
            }
            coordinates.Column = GetCharacterColumn(coordinates.Line, cindex);
        }
    }

    void ImGuiTextEditor::DeleteRange(const Coordinates& start, const Coordinates& end)
    {
        assert(end >= start);
        assert(!_readOnly);
        //printf("D(%d.%d)-(%d.%d)\n", aStart.mLine, aStart.mColumn, aEnd.mLine, aEnd.mColumn);

        if (end == start)
            return;

        auto startIndex = GetCharacterIndex(start);
        auto endIndex = GetCharacterIndex(end);

        if (start.Line == end.Line)
        {
            auto& line = _lines[start.Line];
            auto n = GetLineMaxColumn(start.Line);
            if (end.Column >= n)
                line.erase(line.begin() + startIndex, line.end());
            else
                line.erase(line.begin() + startIndex, line.begin() + endIndex);
        }
        else
        {
            auto& firstLine = _lines[start.Line];
            auto& lastLine = _lines[end.Line];

            firstLine.erase(firstLine.begin() + startIndex, firstLine.end());
            lastLine.erase(lastLine.begin(), lastLine.begin() + endIndex);

            if (start.Line < end.Line)
                firstLine.insert(firstLine.end(), lastLine.begin(), lastLine.end());

            if (start.Line < end.Line)
                RemoveLine(start.Line + 1, end.Line + 1);
        }

        _textChanged = true;
    }

    int ImGuiTextEditor::InsertTextAt(Coordinates& where, const char* value)
    {
        assert(!_readOnly);

        int cindex = GetCharacterIndex(where);
        int totalLines = 0;
        while (*value != '\0')
        {
            assert(!_lines.empty());

            if (*value == '\r')
            {
                // skip
                ++value;
            }
            else if (*value == '\n')
            {
                if (cindex < (int)_lines[where.Line].size())
                {
                    auto& newLine = InsertLine(where.Line + 1);
                    auto& line = _lines[where.Line];
                    newLine.insert(newLine.begin(), line.begin() + cindex, line.end());
                    line.erase(line.begin() + cindex, line.end());
                }
                else
                {
                    InsertLine(where.Line + 1);
                }
                ++where.Line;
                where.Column = 0;
                cindex = 0;
                ++totalLines;
                ++value;
            }
            else
            {
                auto& line = _lines[where.Line];
                auto d = UTF8CharLength(*value);
                while (d-- > 0 && *value != '\0')
                    line.insert(line.begin() + cindex++, Glyph(*value++, PaletteIndex::Default));
                ++where.Column;
            }

            _textChanged = true;
        }

        return totalLines;
    }

    void ImGuiTextEditor::AddUndo(UndoRecord& value)
    {
        assert(!_readOnly);

        _undoBuffer.resize((size_t)(_undoIndex + 1));
        _undoBuffer.back() = value;
        ++_undoIndex;
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::ScreenPosToCoordinates(const ImVec2& position) const
    {
        ImVec2 origin = ImGui::GetCursorScreenPos();
        ImVec2 local(position.x - origin.x, position.y - origin.y);

        int lineNo = std::max(0, (int)floor(local.y / _charAdvance.y));

        int columnCoord = 0;

        if (lineNo >= 0 && lineNo < (int)_lines.size())
        {
            auto& line = _lines.at(lineNo);

            int columnIndex = 0;
            float columnX = 0.0f;

            while ((size_t)columnIndex < line.size())
            {
                float columnWidth = 0.0f;

                if (line[columnIndex].Character == '\t')
                {
                    float spaceSize = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, " ").x;
                    float oldX = columnX;
                    float newColumnX = (1.0f + std::floor((1.0f + columnX) / (float(_tabSize) * spaceSize))) * (float(_tabSize) * spaceSize);
                    columnWidth = newColumnX - oldX;
                    if (_textStart + columnX + columnWidth * 0.5f > local.x)
                        break;
                    columnX = newColumnX;
                    columnCoord = (columnCoord / _tabSize) * _tabSize + _tabSize;
                    columnIndex++;
                }
                else
                {
                    char buf[7];
                    auto d = UTF8CharLength(line[columnIndex].Character);
                    int i = 0;
                    while (i < 6 && d-- > 0)
                        buf[i++] = line[columnIndex++].Character;
                    buf[i] = '\0';
                    columnWidth = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, buf).x;
                    if (_textStart + columnX + columnWidth * 0.5f > local.x)
                        break;
                    columnX += columnWidth;
                    columnCoord++;
                }
            }
        }

        return SanitizeCoordinates(Coordinates(lineNo, columnCoord));
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::FindWordStart(const Coordinates& from) const
    {
        Coordinates at = from;
        if (at.Line >= (int)_lines.size())
            return at;

        auto& line = _lines[at.Line];
        auto cindex = GetCharacterIndex(at);

        if (cindex >= (int)line.size())
            return at;

        while (cindex > 0 && isspace(line[cindex].Character))
            --cindex;

        auto cstart = (PaletteIndex)line[cindex].ColorIndex;
        while (cindex > 0)
        {
            auto c = line[cindex].Character;
            if ((c & 0xC0) != 0x80)    // not UTF code sequence 10xxxxxx
            {
                if (c <= 32 && isspace(c))
                {
                    cindex++;
                    break;
                }
                if (cstart != (PaletteIndex)line[size_t(cindex - 1)].ColorIndex)
                    break;
            }
            --cindex;
        }
        return Coordinates(at.Line, GetCharacterColumn(at.Line, cindex));
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::FindWordEnd(const Coordinates& from) const
    {
        Coordinates at = from;
        if (at.Line >= (int)_lines.size())
            return at;

        auto& line = _lines[at.Line];
        auto cindex = GetCharacterIndex(at);

        if (cindex >= (int)line.size())
            return at;

        bool prevspace = (bool)isspace(line[cindex].Character);
        auto cstart = (PaletteIndex)line[cindex].ColorIndex;
        while (cindex < (int)line.size())
        {
            auto c = line[cindex].Character;
            auto d = UTF8CharLength(c);
            if (cstart != (PaletteIndex)line[cindex].ColorIndex)
                break;

            if (prevspace != !!isspace(c))
            {
                if (isspace(c))
                    while (cindex < (int)line.size() && isspace(line[cindex].Character))
                        ++cindex;
                break;
            }
            cindex += d;
        }
        return Coordinates(from.Line, GetCharacterColumn(from.Line, cindex));
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::FindNextWord(const Coordinates& from) const
    {
        Coordinates at = from;
        if (at.Line >= (int)_lines.size())
            return at;

        // skip to the next non-word character
        auto cindex = GetCharacterIndex(from);
        bool isword = false;
        bool skip = false;
        if (cindex < (int)_lines[at.Line].size())
        {
            auto& line = _lines[at.Line];
            isword = isalnum(line[cindex].Character);
            skip = isword;
        }

        while (!isword || skip)
        {
            if (at.Line >= (int)_lines.size())
            {
                auto l = std::max(0, (int)_lines.size() - 1);
                return Coordinates(l, GetLineMaxColumn(l));
            }

            auto& line = _lines[at.Line];
            if (cindex < (int)line.size())
            {
                isword = isalnum(line[cindex].Character);

                if (isword && !skip)
                    return Coordinates(at.Line, GetCharacterColumn(at.Line, cindex));

                if (!isword)
                    skip = false;

                cindex++;
            }
            else
            {
                cindex = 0;
                ++at.Line;
                skip = false;
                isword = false;
            }
        }

        return at;
    }

    int ImGuiTextEditor::GetCharacterIndex(const Coordinates& coordinates) const
    {
        if (coordinates.Line >= (int)_lines.size())
            return -1;
        auto& line = _lines[coordinates.Line];
        int c = 0;
        int i = 0;
        for (; i < line.size() && c < coordinates.Column;)
        {
            if (line[i].Character == '\t')
                c = (c / _tabSize) * _tabSize + _tabSize;
            else
                ++c;
            i += UTF8CharLength(line[i].Character);
        }
        return i;
    }

    int ImGuiTextEditor::GetCharacterColumn(int line, int index) const
    {
        if (line >= (int)_lines.size())
            return 0;
        auto& currentLine = _lines[line];
        int col = 0;
        int i = 0;
        while (i < index && i < (int)currentLine.size())
        {
            auto c = currentLine[i].Character;
            i += UTF8CharLength(c);
            if (c == '\t')
                col = (col / _tabSize) * _tabSize + _tabSize;
            else
                col++;
        }
        return col;
    }

    int ImGuiTextEditor::GetLineCharacterCount(int line) const
    {
        if (line >= (int)_lines.size())
            return 0;
        auto& currentLine = _lines[line];
        int c = 0;
        for (unsigned i = 0; i < currentLine.size(); c++)
            i += UTF8CharLength(currentLine[i].Character);
        return c;
    }

    int ImGuiTextEditor::GetLineMaxColumn(int line) const
    {
        if (line >= (int)_lines.size())
            return 0;
        auto& currentLine = _lines[line];
        int col = 0;
        for (unsigned i = 0; i < currentLine.size(); )
        {
            auto c = currentLine[i].Character;
            if (c == '\t')
                col = (col / _tabSize) * _tabSize + _tabSize;
            else
                col++;
            i += UTF8CharLength(c);
        }
        return col;
    }

    bool ImGuiTextEditor::IsOnWordBoundary(const Coordinates& at) const
    {
        if (at.Line >= (int)_lines.size() || at.Column == 0)
            return true;

        auto& line = _lines[at.Line];
        auto cindex = GetCharacterIndex(at);
        if (cindex >= (int)line.size())
            return true;

        if (_colorizerEnabled)
            return line[cindex].ColorIndex != line[size_t(cindex - 1)].ColorIndex;

        return isspace(line[cindex].Character) != isspace(line[cindex - 1].Character);
    }

    void ImGuiTextEditor::RemoveLine(int start, int end)
    {
        assert(!_readOnly);
        assert(end >= start);
        assert(_lines.size() > (size_t)(end - start));

        ErrorMarkers etmp;
        for (auto& i : _errorMarkers)
        {
            ErrorMarkers::value_type e(i.first >= start ? i.first - 1 : i.first, i.second);
            if (e.first >= start && e.first <= end)
                continue;
            etmp.insert(e);
        }
        _errorMarkers = std::move(etmp);

        Breakpoints btmp;
        for (const auto& i : _breakpoints)
        {
            if (i >= start && i <= end)
                continue;
            btmp.insert(i >= start ? i - 1 : i);
        }
        _breakpoints = std::move(btmp);

        _lines.erase(_lines.begin() + start, _lines.begin() + end);
        assert(!_lines.empty());

        _textChanged = true;
    }

    void ImGuiTextEditor::RemoveLine(int index)
    {
        assert(!_readOnly);
        assert(_lines.size() > 1);

        ErrorMarkers etmp;
        for (auto& i : _errorMarkers)
        {
            ErrorMarkers::value_type e(i.first > index ? i.first - 1 : i.first, i.second);
            if (e.first - 1 == index)
                continue;
            etmp.insert(e);
        }
        _errorMarkers = std::move(etmp);

        Breakpoints btmp;
        for (const auto& i : _breakpoints)
        {
            if (i == index)
                continue;
            btmp.insert(i >= index ? i - 1 : i);
        }
        _breakpoints = std::move(btmp);

        _lines.erase(_lines.begin() + index);
        assert(!_lines.empty());

        _textChanged = true;
    }

    ImGuiTextEditor::Line& ImGuiTextEditor::InsertLine(int index)
    {
        assert(!_readOnly);

        auto& result = *_lines.insert(_lines.begin() + index, Line());

        ErrorMarkers etmp;
        for (auto& i : _errorMarkers)
            etmp.insert(ErrorMarkers::value_type(i.first >= index ? i.first + 1 : i.first, i.second));
        _errorMarkers = std::move(etmp);

        Breakpoints btmp;
        for (const auto& i : _breakpoints)
            btmp.insert(i >= index ? i + 1 : i);
        _breakpoints = std::move(btmp);

        return result;
    }

    void ImGuiTextEditor::EnterCharacter(ImWchar character, bool shift)
    {
        assert(!_readOnly);

        UndoRecord u;

        u._before = _state;

        if (HasSelection())
        {
            if (character == '\t' && _state.SelectionStart.Line != _state.SelectionEnd.Line)
            {

                auto start = _state.SelectionStart;
                auto end = _state.SelectionEnd;
                auto originalEnd = end;

                if (start > end)
                    std::swap(start, end);
                start.Column = 0;
                //            end.mColumn = end.mLine < mLines.size() ? mLines[end.mLine].size() : 0;
                if (end.Column == 0 && end.Line > 0)
                    --end.Line;
                if (end.Line >= (int)_lines.size())
                    end.Line = _lines.empty() ? 0 : (int)_lines.size() - 1;
                end.Column = GetLineMaxColumn(end.Line);

                //if (end.mColumn >= GetLineMaxColumn(end.mLine))
                //    end.mColumn = GetLineMaxColumn(end.mLine) - 1;

                u._removedStart = start;
                u._removedEnd = end;
                u._removed = GetText(start, end);

                bool modified = false;

                for (int i = start.Line; i <= end.Line; i++)
                {
                    auto& line = _lines[i];
                    if (shift)
                    {
                        if (!line.empty())
                        {
                            if (line.front().Character == '\t')
                            {
                                line.erase(line.begin());
                                modified = true;
                            }
                            else
                            {
                                for (int j = 0; j < _tabSize && !line.empty() && line.front().Character == ' '; j++)
                                {
                                    line.erase(line.begin());
                                    modified = true;
                                }
                            }
                        }
                    }
                    else
                    {
                        line.insert(line.begin(), Glyph('\t', ImGuiTextEditor::PaletteIndex::Background));
                        modified = true;
                    }
                }

                if (modified)
                {
                    start = Coordinates(start.Line, GetCharacterColumn(start.Line, 0));
                    Coordinates rangeEnd;
                    if (originalEnd.Column != 0)
                    {
                        end = Coordinates(end.Line, GetLineMaxColumn(end.Line));
                        rangeEnd = end;
                        u._added = GetText(start, end);
                    }
                    else
                    {
                        end = Coordinates(originalEnd.Line, 0);
                        rangeEnd = Coordinates(end.Line - 1, GetLineMaxColumn(end.Line - 1));
                        u._added = GetText(start, rangeEnd);
                    }

                    u._addedStart = start;
                    u._addedEnd = rangeEnd;
                    u._after = _state;

                    _state.SelectionStart = start;
                    _state.SelectionEnd = end;
                    AddUndo(u);

                    _textChanged = true;

                    EnsureCursorVisible();
                }

                return;
            } // c == '\t'
            else
            {
                u._removed = GetSelectedText();
                u._removedStart = _state.SelectionStart;
                u._removedEnd = _state.SelectionEnd;
                DeleteSelection();
            }
        } // HasSelection

        auto coord = GetActualCursorCoordinates();
        u._addedStart = coord;

        assert(!_lines.empty());

        if (character == '\n')
        {
            InsertLine(coord.Line + 1);
            auto& line = _lines[coord.Line];
            auto& newLine = _lines[coord.Line + 1];

            if (_languageDefinition._autoIndentation)
                for (size_t it = 0; it < line.size() && isascii(line[it].Character) && isblank(line[it].Character); ++it)
                    newLine.push_back(line[it]);

            const size_t whitespaceSize = newLine.size();
            auto cindex = GetCharacterIndex(coord);
            newLine.insert(newLine.end(), line.begin() + cindex, line.end());
            line.erase(line.begin() + cindex, line.begin() + line.size());
            SetCursorPosition(Coordinates(coord.Line + 1, GetCharacterColumn(coord.Line + 1, (int)whitespaceSize)));
            u._added = (char)character;
        }
        else
        {
            char buf[7];
            int e = ImTextCharToUtf8(buf, 7, character);
            if (e > 0)
            {
                buf[e] = '\0';
                auto& line = _lines[coord.Line];
                auto cindex = GetCharacterIndex(coord);

                if (_overwrite && cindex < (int)line.size())
                {
                    auto d = UTF8CharLength(line[cindex].Character);

                    u._removedStart = _state.CursorPosition;
                    u._removedEnd = Coordinates(coord.Line, GetCharacterColumn(coord.Line, cindex + d));

                    while (d-- > 0 && cindex < (int)line.size())
                    {
                        u._removed += line[cindex].Character;
                        line.erase(line.begin() + cindex);
                    }
                }

                for (auto p = buf; *p != '\0'; p++, ++cindex)
                    line.insert(line.begin() + cindex, Glyph(*p, PaletteIndex::Default));
                u._added = buf;

                SetCursorPosition(Coordinates(coord.Line, GetCharacterColumn(coord.Line, cindex)));
            }
            else
                return;
        }

        _textChanged = true;

        u._addedEnd = GetActualCursorCoordinates();
        u._after = _state;

        AddUndo(u);

        Colorize(coord.Line - 1, 3);
        EnsureCursorVisible();
    }

    void ImGuiTextEditor::Backspace()
    {
        assert(!_readOnly);

        if (_lines.empty())
            return;

        UndoRecord u;
        u._before = _state;

        if (HasSelection())
        {
            u._removed = GetSelectedText();
            u._removedStart = _state.SelectionStart;
            u._removedEnd = _state.SelectionEnd;

            DeleteSelection();
        }
        else
        {
            auto pos = GetActualCursorCoordinates();
            SetCursorPosition(pos);

            if (_state.CursorPosition.Column == 0)
            {
                if (_state.CursorPosition.Line == 0)
                    return;

                u._removed = '\n';
                u._removedStart = u._removedEnd = Coordinates(pos.Line - 1, GetLineMaxColumn(pos.Line - 1));
                Advance(u._removedEnd);

                auto& line = _lines[_state.CursorPosition.Line];
                auto& prevLine = _lines[_state.CursorPosition.Line - 1];
                auto prevSize = GetLineMaxColumn(_state.CursorPosition.Line - 1);
                prevLine.insert(prevLine.end(), line.begin(), line.end());

                ErrorMarkers etmp;
                for (auto& i : _errorMarkers)
                    etmp.insert(ErrorMarkers::value_type(i.first - 1 == _state.CursorPosition.Line ? i.first - 1 : i.first, i.second));
                _errorMarkers = std::move(etmp);

                RemoveLine(_state.CursorPosition.Line);
                --_state.CursorPosition.Line;
                _state.CursorPosition.Column = prevSize;
            }
            else
            {
                auto& line = _lines[_state.CursorPosition.Line];
                auto cindex = GetCharacterIndex(pos) - 1;
                auto cend = cindex + 1;
                while (cindex > 0 && IsUTFSequence(line[cindex].Character))
                    --cindex;

                //if (cindex > 0 && UTF8CharLength(line[cindex].mChar) > 1)
                //    --cindex;

                u._removedStart = u._removedEnd = GetActualCursorCoordinates();
                --u._removedStart.Column;
                --_state.CursorPosition.Column;

                while ((size_t)cindex < line.size() && cend-- > cindex)
                {
                    u._removed += line[cindex].Character;
                    line.erase(line.begin() + cindex);
                }
            }

            _textChanged = true;

            EnsureCursorVisible();
            Colorize(_state.CursorPosition.Line, 1);
        }

        u._after = _state;
        AddUndo(u);
    }

    void ImGuiTextEditor::DeleteSelection()
    {
        assert(_state.SelectionEnd >= _state.SelectionStart);

        if (_state.SelectionEnd == _state.SelectionStart)
            return;

        DeleteRange(_state.SelectionStart, _state.SelectionEnd);

        SetSelection(_state.SelectionStart, _state.SelectionStart);
        SetCursorPosition(_state.SelectionStart);
        Colorize(_state.SelectionStart.Line, 1);
    }

    String ImGuiTextEditor::GetWordUnderCursor() const
    {
        auto c = GetCursorPosition();
        return GetWordAt(c);
    }

    String ImGuiTextEditor::GetWordAt(const Coordinates& coords) const
    {
        auto start = FindWordStart(coords);
        auto end = FindWordEnd(coords);

        std::string r;

        auto istart = GetCharacterIndex(start);
        auto iend = GetCharacterIndex(end);

        for (auto it = istart; it < iend; ++it)
            r.push_back(_lines[coords.Line][it].Character);

        return r;
    }

    ImU32 ImGuiTextEditor::GetGlyphColor(const Glyph& glyph) const
    {
        if (!_colorizerEnabled)
            return _palette[(int)PaletteIndex::Default];
        if (glyph.Comment)
            return _palette[(int)PaletteIndex::Comment];
        if (glyph.MultiLineComment)
            return _palette[(int)PaletteIndex::MultiLineComment];
        auto const color = _palette[(int)glyph.ColorIndex];
        if (glyph.Preprocessor)
        {
            const auto ppcolor = _palette[(int)PaletteIndex::Preprocessor];
            const int c0 = ((ppcolor & 0xff) + (color & 0xff)) / 2;
            const int c1 = (((ppcolor >> 8) & 0xff) + ((color >> 8) & 0xff)) / 2;
            const int c2 = (((ppcolor >> 16) & 0xff) + ((color >> 16) & 0xff)) / 2;
            const int c3 = (((ppcolor >> 24) & 0xff) + ((color >> 24) & 0xff)) / 2;
            return ImU32(c0 | (c1 << 8) | (c2 << 16) | (c3 << 24));
        }
        return color;
    }

    float ImGuiTextEditor::GetFontSize() const
    {
        return ImGui::GetFontSize();
    }

    void ImGuiTextEditor::HandleKeyboardInputs()
    {
        ImGuiIO& io = ImGui::GetIO();
        auto shift = io.KeyShift;
        auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
        auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

        if (ImGui::IsWindowFocused())
        {
            if (ImGui::IsWindowHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
            //ImGui::CaptureKeyboardFromApp(true);

            io.WantCaptureKeyboard = true;
            io.WantTextInput = true;

            if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
                Undo();
            else if (!IsReadOnly() && !ctrl && !shift && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
                Undo();
            else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
                Redo();
            else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
                MoveUp(1, shift);
            else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
                MoveDown(1, shift);
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
                MoveLeft(1, shift, ctrl);
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
                MoveRight(1, shift, ctrl);
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp)))
                MoveUp(GetPageSize() - 4, shift);
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown)))
                MoveDown(GetPageSize() - 4, shift);
            else if (!alt && ctrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
                MoveTop(shift);
            else if (ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
                MoveBottom(shift);
            else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
                MoveHome(shift);
            else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
                MoveEnd(shift);
            else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
                Delete();
            else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
                Backspace();
            else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
                _overwrite ^= true;
            else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
                Copy();
            else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
                Copy();
            else if (!IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
                Paste();
            else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
                Paste();
            else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
                Cut();
            else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
                Cut();
            else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
                SelectAll();
            else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
                EnterCharacter('\n', false);
            else if (!IsReadOnly() && !ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
                EnterCharacter('\t', shift);

            if (!IsReadOnly() && !io.InputQueueCharacters.empty())
            {
                for (int i = 0; i < io.InputQueueCharacters.Size; i++)
                {
                    auto c = io.InputQueueCharacters[i];
                    if (c != 0 && (c == '\n' || c >= 32))
                        EnterCharacter(c, shift);
                }
                io.InputQueueCharacters.resize(0);
            }
        }
    }

    void ImGuiTextEditor::HandleMouseInputs()
    {
        ImGuiIO& io = ImGui::GetIO();
        auto shift = io.KeyShift;
        auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
        auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

        if (ImGui::IsWindowHovered())
        {
            if (!shift && !alt)
            {
                auto click = ImGui::IsMouseClicked(0);
                auto doubleClick = ImGui::IsMouseDoubleClicked(0);
                auto t = ImGui::GetTime();
                auto tripleClick = click && !doubleClick && (_lastClick != -1.0f && (t - _lastClick) < io.MouseDoubleClickTime);

                /*
                Left mouse button triple click
                */

                if (tripleClick)
                {
                    if (!ctrl)
                    {
                        _state.CursorPosition = _interactiveStart = _interactiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
                        _selectionMode = SelectionMode::Line;
                        SetSelection(_interactiveStart, _interactiveEnd, _selectionMode);
                    }

                    _lastClick = -1.0f;
                }

                /*
                Left mouse button double click
                */

                else if (doubleClick)
                {
                    if (!ctrl)
                    {
                        _state.CursorPosition = _interactiveStart = _interactiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
                        if (_selectionMode == SelectionMode::Line)
                            _selectionMode = SelectionMode::Normal;
                        else
                            _selectionMode = SelectionMode::Word;
                        SetSelection(_interactiveStart, _interactiveEnd, _selectionMode);
                    }

                    _lastClick = (float)ImGui::GetTime();
                }

                /*
                Left mouse button click
                */
                else if (click)
                {
                    _state.CursorPosition = _interactiveStart = _interactiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
                    if (ctrl)
                        _selectionMode = SelectionMode::Word;
                    else
                        _selectionMode = SelectionMode::Normal;
                    SetSelection(_interactiveStart, _interactiveEnd, _selectionMode);

                    _lastClick = (float)ImGui::GetTime();
                }
                // Mouse left button dragging (=> update selection)
                else if (ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
                {
                    io.WantCaptureMouse = true;
                    _state.CursorPosition = _interactiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
                    SetSelection(_interactiveStart, _interactiveEnd, _selectionMode);
                }
            }
        }
    }

    void ImGuiTextEditor::Render()
    {
        /* Compute mCharAdvance regarding to scaled font size (Ctrl + mouse wheel)*/
        const float fontSize = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
        _charAdvance = ImVec2(fontSize, ImGui::GetTextLineHeightWithSpacing() * _lineSpacing);

        /* Update palette with the current alpha from style */
        for (int i = 0; i < (int)PaletteIndex::Max; ++i)
        {
            auto color = ImGui::ColorConvertU32ToFloat4(_paletteBase[i]);
            color.w *= ImGui::GetStyle().Alpha;
            _palette[i] = ImGui::ColorConvertFloat4ToU32(color);
        }

        assert(_lineBuffer.empty());

        auto contentSize = ImGui::GetWindowContentRegionMax();
        auto drawList = ImGui::GetWindowDrawList();
        float longest(_textStart);

        if (_scrollToTop)
        {
            _scrollToTop = false;
            ImGui::SetScrollY(0.f);
        }

        ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
        auto scrollX = ImGui::GetScrollX();
        auto scrollY = ImGui::GetScrollY();

        auto lineNo = (int)floor(scrollY / _charAdvance.y);
        auto globalLineMax = (int)_lines.size();
        auto lineMax = std::max(0, std::min((int)_lines.size() - 1, lineNo + (int)floor((scrollY + contentSize.y) / _charAdvance.y)));

        // Deduce mTextStart by evaluating mLines size (global lineMax) plus two spaces as text width
        char buf[16];
        snprintf(buf, 16, " %d ", globalLineMax);
        _textStart = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr).x + _leftMargin;

        if (!_lines.empty())
        {
            float spaceSize = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;

            while (lineNo <= lineMax)
            {
                ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + lineNo * _charAdvance.y);
                ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + _textStart, lineStartScreenPos.y);

                auto& line = _lines[lineNo];
                longest = std::max(_textStart + TextDistanceToLineStart(Coordinates(lineNo, GetLineMaxColumn(lineNo))), longest);
                auto columnNo = 0;
                Coordinates lineStartCoord(lineNo, 0);
                Coordinates lineEndCoord(lineNo, GetLineMaxColumn(lineNo));

                // Draw selection for the current line
                float sstart = -1.0f;
                float ssend = -1.0f;

                assert(_state.SelectionStart <= _state.SelectionEnd);
                if (_state.SelectionStart <= lineEndCoord)
                    sstart = _state.SelectionStart > lineStartCoord ? TextDistanceToLineStart(_state.SelectionStart) : 0.0f;
                if (_state.SelectionEnd > lineStartCoord)
                    ssend = TextDistanceToLineStart(_state.SelectionEnd < lineEndCoord ? _state.SelectionEnd : lineEndCoord);

                if (_state.SelectionEnd.Line > lineNo)
                    ssend += _charAdvance.x;

                if (sstart != -1 && ssend != -1 && sstart < ssend)
                {
                    ImVec2 vstart(lineStartScreenPos.x + _textStart + sstart, lineStartScreenPos.y);
                    ImVec2 vend(lineStartScreenPos.x + _textStart + ssend, lineStartScreenPos.y + _charAdvance.y);
                    drawList->AddRectFilled(vstart, vend, _palette[(int)PaletteIndex::Selection]);
                }

                // Draw breakpoints
                auto start = ImVec2(lineStartScreenPos.x + scrollX, lineStartScreenPos.y);

                if (_breakpoints.count(lineNo + 1) != 0)
                {
                    auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + _charAdvance.y);
                    drawList->AddRectFilled(start, end, _palette[(int)PaletteIndex::Breakpoint]);
                }

                // Draw error markers
                auto errorIt = _errorMarkers.find(lineNo + 1);
                if (errorIt != _errorMarkers.end())
                {
                    auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + _charAdvance.y);
                    drawList->AddRectFilled(start, end, _palette[(int)PaletteIndex::ErrorMarker]);

                    if (ImGui::IsMouseHoveringRect(lineStartScreenPos, end))
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                        ImGui::Text("Error at line %d:", errorIt->first);
                        ImGui::PopStyleColor();
                        ImGui::Separator();
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
                        ImGui::Text("%s", errorIt->second.c_str());
                        ImGui::PopStyleColor();
                        ImGui::EndTooltip();
                    }
                }

                // Draw line number (right aligned)
                snprintf(buf, 16, "%d  ", lineNo + 1);

                auto lineNoWidth = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr).x;
                drawList->AddText(ImVec2(lineStartScreenPos.x + _textStart - lineNoWidth, lineStartScreenPos.y), _palette[(int)PaletteIndex::LineNumber], buf);

                if (_state.CursorPosition.Line == lineNo)
                {
                    auto focused = ImGui::IsWindowFocused();

                    // Highlight the current line (where the cursor is)
                    if (!HasSelection())
                    {
                        auto end = ImVec2(start.x + contentSize.x + scrollX, start.y + _charAdvance.y);
                        drawList->AddRectFilled(start, end, _palette[(int)(focused ? PaletteIndex::CurrentLineFill : PaletteIndex::CurrentLineFillInactive)]);
                        drawList->AddRect(start, end, _palette[(int)PaletteIndex::CurrentLineEdge], 1.0f);
                    }

                    // Render the cursor
                    if (focused)
                    {
                        auto timeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        auto elapsed = timeEnd - _startTime;
                        if (elapsed > 400)
                        {
                            float width = 1.0f;
                            auto cindex = GetCharacterIndex(_state.CursorPosition);
                            float cx = TextDistanceToLineStart(_state.CursorPosition);

                            if (_overwrite && cindex < (int)line.size())
                            {
                                auto c = line[cindex].Character;
                                if (c == '\t')
                                {
                                    auto x = (1.0f + std::floor((1.0f + cx) / (float(_tabSize) * spaceSize))) * (float(_tabSize) * spaceSize);
                                    width = x - cx;
                                }
                                else
                                {
                                    char buf2[2];
                                    buf2[0] = line[cindex].Character;
                                    buf2[1] = '\0';
                                    width = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, buf2).x;
                                }
                            }
                            ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);
                            ImVec2 cend(textScreenPos.x + cx + width, lineStartScreenPos.y + _charAdvance.y);
                            drawList->AddRectFilled(cstart, cend, _palette[(int)PaletteIndex::Cursor]);
                            if (elapsed > 800)
                                _startTime = timeEnd;
                        }
                    }
                }

                // Render colorized text
                auto prevColor = line.empty() ? _palette[(int)PaletteIndex::Default] : GetGlyphColor(line[0]);
                ImVec2 bufferOffset;

                for (int i = 0; i < (int)line.size();)
                {
                    auto& glyph = line[i];
                    auto color = GetGlyphColor(glyph);

                    if ((color != prevColor || glyph.Character == '\t' || glyph.Character == ' ') && !_lineBuffer.empty())
                    {
                        const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
                        drawList->AddText(newOffset, prevColor, _lineBuffer.c_str());
                        auto textSize = ImGui::GetFont()->CalcTextSizeA(GetFontSize(), FLT_MAX, -1.0f, _lineBuffer.c_str(), nullptr, nullptr);
                        bufferOffset.x += textSize.x;
                        _lineBuffer.clear();
                    }
                    prevColor = color;

                    if (glyph.Character == '\t')
                    {
                        auto oldX = bufferOffset.x;
                        bufferOffset.x = (1.0f + std::floor((1.0f + bufferOffset.x) / (float(_tabSize) * spaceSize))) * (float(_tabSize) * spaceSize);
                        ++i;

                        if (_showWhitespaces)
                        {
                            const auto s = GetFontSize();
                            const auto x1 = textScreenPos.x + oldX + 1.0f;
                            const auto x2 = textScreenPos.x + bufferOffset.x - 1.0f;
                            const auto y = textScreenPos.y + bufferOffset.y + s * 0.5f;
                            const ImVec2 p1(x1, y);
                            const ImVec2 p2(x2, y);
                            const ImVec2 p3(x2 - s * 0.2f, y - s * 0.2f);
                            const ImVec2 p4(x2 - s * 0.2f, y + s * 0.2f);
                            drawList->AddLine(p1, p2, 0x90909090);
                            drawList->AddLine(p2, p3, 0x90909090);
                            drawList->AddLine(p2, p4, 0x90909090);
                        }
                    }
                    else if (glyph.Character == ' ')
                    {
                        if (_showWhitespaces)
                        {
                            const auto s = GetFontSize();
                            const auto x = textScreenPos.x + bufferOffset.x + spaceSize * 0.5f;
                            const auto y = textScreenPos.y + bufferOffset.y + s * 0.5f;
                            drawList->AddCircleFilled(ImVec2(x, y), 1.5f, 0x80808080, 4);
                        }
                        bufferOffset.x += spaceSize;
                        i++;
                    }
                    else
                    {
                        auto l = UTF8CharLength(glyph.Character);
                        while (l-- > 0)
                            _lineBuffer.push_back(line[i++].Character);
                    }
                    ++columnNo;
                }

                if (!_lineBuffer.empty())
                {
                    const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
                    drawList->AddText(newOffset, prevColor, _lineBuffer.c_str());
                    _lineBuffer.clear();
                }

                ++lineNo;
            }

            // Draw a tooltip on known identifiers/preprocessor symbols
            if (ImGui::IsMousePosValid())
            {
                auto id = GetWordAt(ScreenPosToCoordinates(ImGui::GetMousePos()));
                if (!id.empty())
                {
                    auto it = _languageDefinition._identifiers.find(id);
                    if (it != _languageDefinition._identifiers.end())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(it->second.Declaration.c_str());
                        ImGui::EndTooltip();
                    }
                    else
                    {
                        auto pi = _languageDefinition._preprocIdentifiers.find(id);
                        if (pi != _languageDefinition._preprocIdentifiers.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(pi->second.Declaration.c_str());
                            ImGui::EndTooltip();
                        }
                    }
                }
            }
        }


        ImGui::Dummy(ImVec2((longest + 2), _lines.size() * _charAdvance.y));

        if (_scrollToCursor)
        {
            EnsureCursorVisible();
            ImGui::SetWindowFocus();
            _scrollToCursor = false;
        }
    }
}
