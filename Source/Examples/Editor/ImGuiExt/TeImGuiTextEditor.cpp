#include "TeImGuiTextEditor.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

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
        , _checkComments(true)
        , _lastClick(-1.0f)
        , _handleKeyboardInputs(true)
        , _handleMouseInputs(true)
        , _ignoreImGuiChild(false)
        , _showWhitespaces(true)
        , _startTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
    {
        SetPalette(GetDarkPalette());
        SetLanguageDefinition(LanguageDefinition::CPlusPlus());
        _lines.push_back(Line());
    }

    void ImGuiTextEditor::SetLanguageDefinition(const LanguageDefinition& languageDef)
    {
        // TODO
    }

    void ImGuiTextEditor::SetPalette(const Palette& value)
    {
        _paletteBase = value;
    }

    String ImGuiTextEditor::GetSelectedText() const
    {
        return ""; // TODO
    }

    String ImGuiTextEditor::GetCurrentLineText()const
    {
        return ""; // TODO
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
            //EnsureCursorVisible(); TODO
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
        // TODO
    }

    void ImGuiTextEditor::MoveUp(int amount, bool select)
    {
        // TODO
    }

    void ImGuiTextEditor::MoveDown(int amount, bool select)
    {
        // TODO
    }

    void ImGuiTextEditor::MoveLeft(int zmount, bool select, bool wordMode)
    {
        // TODO
    }

    void ImGuiTextEditor::MoveRight(int amount, bool select, bool wordMode)
    {
        // TODO
    }

    void ImGuiTextEditor::MoveTop(bool select)
    {
        // TODO
    }

    void ImGuiTextEditor::MoveBottom(bool select)
    {
        // TODO
    }

    void ImGuiTextEditor::MoveHome(bool select)
    {
        // TODO
    }

    void ImGuiTextEditor::MoveEnd(bool select)
    {
        // TODO
    }

    void ImGuiTextEditor::SetSelectionStart(const Coordinates& position)
    {
        // TODO
    }

    void ImGuiTextEditor::SetSelectionEnd(const Coordinates& position)
    {
        // TODO
    }

    void ImGuiTextEditor::SetSelection(const Coordinates& start, const Coordinates& end, SelectionMode mode)
    {
        // TODO
    }

    void ImGuiTextEditor::SelectWordUnderCursor()
    {
        // TODO
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
        // TODO
    }

    void ImGuiTextEditor::Cut()
    {
        // TODO
    }

    void ImGuiTextEditor::Paste()
    {
        // TODO
    }

    void ImGuiTextEditor::Delete()
    {
        // TODO
    }

    bool ImGuiTextEditor::CanUndo() const
    {
        return false; // TODO
    }

    bool ImGuiTextEditor::CanRedo() const
    {
        return false; // TODO
    }

    void ImGuiTextEditor::Undo(int steps)
    {
        // TODO
    }

    void ImGuiTextEditor::Redo(int steps)
    {
        // TODO
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
                0xff7f7f7f,    // None
                0xffff0c06,    // Keyword    
                0xff008000,    // Number
                0xff2020a0,    // String
                0xff304070, // Char literal
                0xff000000, // Punctuation
                0xff406060,    // Preprocessor
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
                0xff00ffff,    // None
                0xffffff00,    // Keyword    
                0xff00ff00,    // Number
                0xff808000,    // String
                0xff808000, // Char literal
                0xffffffff, // Punctuation
                0xff008000,    // Preprocessor
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
        // TODO
    }

    void ImGuiTextEditor::UndoRecord::Redo(ImGuiTextEditor* editor)
    {
        // TODO
    }

    void ImGuiTextEditor::Render(const char* aTitle, const ImVec2& aSize, bool aBorder)
    {
        // TODO
    }

    void ImGuiTextEditor::SetText(const String& aText)
    {
        // TODO
    }

    String ImGuiTextEditor::GetText() const
    {
        return GetText(Coordinates(), Coordinates((int)_lines.size(), 0));
    }

    void ImGuiTextEditor::SetTextLines(const Vector<String>& aLines)
    {
        // TODO
    }

    Vector<String> ImGuiTextEditor::GetTextLines() const
    {
        Vector<String> result;

        result.reserve(_lines.size());

        for (auto& line : _lines)
        {
            std::string text;

            text.resize(line.size());

            for (size_t i = 0; i < line.size(); ++i)
                text[i] = line[i].Character;

            result.emplace_back(std::move(text));
        }

        return result;
    }

    void ImGuiTextEditor::ProcessInputs()
    {
        // TODO
    }

    void ImGuiTextEditor::Colorize(int fromLine, int count)
    {
        // TODO
    }

    void ImGuiTextEditor::ColorizeRange(int fromLine, int toLine)
    {
        // TODO
    }

    void ImGuiTextEditor::ColorizeInternal()
    {
        // TODO
    }

    float ImGuiTextEditor::TextDistanceToLineStart(const Coordinates& from) const
    {
        return 0.0f; // TODO
    }

    void ImGuiTextEditor::EnsureCursorVisible()
    {
        // TODO
    }

    int ImGuiTextEditor::GetPageSize() const
    {
        return 0; // TODO
    }

    String ImGuiTextEditor::GetText(const Coordinates& start, const Coordinates& end) const
    {
        return ""; // TODO
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
        // TODO
    }

    void ImGuiTextEditor::DeleteRange(const Coordinates& start, const Coordinates& end)
    {
        // TODO
    }

    int ImGuiTextEditor::InsertTextAt(Coordinates& where, const char* value)
    {
        return 0; // TODO
    }

    void ImGuiTextEditor::AddUndo(UndoRecord& value)
    {
        // TODO
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::ScreenPosToCoordinates(const ImVec2& position) const
    {
        return Coordinates(0, 0); // TODO
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::FindWordStart(const Coordinates& from) const
    {
        return Coordinates(0, 0); // TODO
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::FindWordEnd(const Coordinates& from) const
    {
        return Coordinates(0, 0); // TODO
    }

    ImGuiTextEditor::Coordinates ImGuiTextEditor::FindNextWord(const Coordinates& from) const
    {
        return Coordinates(0, 0); // TODO
    }

    int ImGuiTextEditor::GetCharacterIndex(const Coordinates& coordinates) const
    {
        return 0; // TODO
    }

    int ImGuiTextEditor::GetCharacterColumn(int line, int index) const
    {
        return 0; // TODO
    }

    int ImGuiTextEditor::GetLineCharacterCount(int line) const
    {
        return 0; // TODO
    }

    int ImGuiTextEditor::GetLineMaxColumn(int line) const
    {
        if (line >= _lines.size())
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
        return false; // TODO
    }

    void ImGuiTextEditor::RemoveLine(int start, int end)
    {
        // TODO
    }

    void ImGuiTextEditor::RemoveLine(int index)
    {
        // TODO
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
        for (auto i : _breakpoints)
            btmp.insert(i >= index ? i + 1 : i);
        _breakpoints = std::move(btmp);

        return result;
    }

    void ImGuiTextEditor::EnterCharacter(ImWchar chararacter, bool shift)
    {
        // TODO
    }

    void ImGuiTextEditor::Backspace()
    {
        // TODO
    }

    void ImGuiTextEditor::DeleteSelection()
    {
        // TODO
    }

    String ImGuiTextEditor::GetWordUnderCursor() const
    {
        return ""; // TODO
    }

    String ImGuiTextEditor::GetWordAt(const Coordinates& coords) const
    {
        return ""; // TODO
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

    void ImGuiTextEditor::HandleKeyboardInputs()
    {
        // TODO
    }

    void ImGuiTextEditor::HandleMouseInputs()
    {
        // TODO
    }

    void ImGuiTextEditor::Render()
    {
        // TODO
    }
}
