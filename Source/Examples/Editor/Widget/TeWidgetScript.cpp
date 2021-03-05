#include "TeWidgetScript.h"
#include "../TeEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "TeCoreApplication.h"
#include "Gui/TeGuiAPI.h"
#include "Components/TeCScript.h"
#include "Scripting/TeScriptManager.h"
#include "Utility/TeFileStream.h"

namespace te
{
    static bool TokenizeCStyleString(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        return false;
    }

    static bool TokenizeCStyleCharacterLiteral(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        return false;
    }

    static bool TokenizeCStyleIdentifier(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        return false;
    }

    static bool TokenizeCStyleNumber(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        return false;
    }

    static bool TokenizeCStylePunctuation(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
    {
        return false;
    }

    const WidgetScript::LanguageDefinition& WidgetScript::LanguageDefinition::CPlusPlus()
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
                langDef._identifiers.insert(std::make_pair(std::string(k), id));
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

    WidgetScript::WidgetScript()
        : Widget(WidgetType::Script)
        , _selections(gEditor().GetSelectionData())
        , _needsUpdateContent(false)
        , _needsSave(false)
    {
        _title = SCRIPT_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove;
    }

    WidgetScript::~WidgetScript()
    { }

    void WidgetScript::Initialize()
    { }

    void WidgetScript::Update()
    {
        if (_selections.ClickedComponent && _selections.ClickedComponent->GetCoreType() == TID_CScript)
        {
            SPtr<CScript> script = std::static_pointer_cast<CScript>(_selections.ClickedComponent);
            String name = script->GetNativeScriptName();
            String path = script->GetNativeScriptPath();

            if (name.empty())
                return;

            if (name != _currentScriptName) // Update current script name
            {
                _currentScriptName = name;
                _currentScriptPath = path;
                _needsUpdateContent = true;
            }

            if (_needsUpdateContent) // If current script name different from previous, we need to load its content
            {
                FileStream file(path + _currentScriptName + ".cpp"); // TODO
                _editorContent = file.GetAsString();
                file.Close();

                _needsUpdateContent = false;
                _needsSave = true;
            }
        }
        else if(!ImGui::IsWindowFocused())
        {
            SaveAndCompile();
            _needsUpdateContent = true;
        }
    }

    void WidgetScript::UpdateBackground()
    {
        SaveAndCompile();
        _needsUpdateContent = true;
    }

    void WidgetScript::SaveAndCompile()
    {
        if (_needsUpdateContent == false && !_currentScriptName.empty() && _needsSave)
        {
            String path = _currentScriptPath + _currentScriptName + ".cpp"; // TODO
            FileStream file(path);
            file.Write((void*)(_editorContent.c_str()), _editorContent.size());
            file.Close();

            gScriptManager().OnMonitorFileModified(path);

            _needsSave = false;
        }
    }
}
