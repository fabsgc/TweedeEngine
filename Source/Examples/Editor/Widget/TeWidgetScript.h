#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"
#include "ImGui/imgui.h"

#include <regex>

namespace te
{
    class WidgetScript : public Widget
    {
    public:
        enum class PaletteIndex
        {
            Default,
            Keyword,
            Number,
            String,
            CharLiteral,
            Punctuation,
            Preprocessor,
            Identifier,
            KnownIdentifier,
            PreprocIdentifier,
            Comment,
            MultiLineComment,
            Background,
            Cursor,
            Selection,
            ErrorMarker,
            Breakpoint,
            LineNumber,
            CurrentLineFill,
            CurrentLineFillInactive,
            CurrentLineEdge,
            Max
        };

        enum class SelectionMode
        {
            Normal,
            Word,
            Line
        };

        struct Breakpoint
        {
            int Line;
            bool Enabled;
            String Condition;

            Breakpoint()
                : Line(-1)
                , Enabled(false)
            {}
        };

        // Represents a character coordinate from the user's point of view,
        // i. e. consider an uniform grid (assuming fixed-width font) on the
        // screen as it is rendered, and each cell has its own coordinate, starting from 0.
        // Tabs are counted as [1..mTabSize] count empty spaces, depending on
        // how many space is necessary to reach the next tab stop.
        // For example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when mTabSize = 4,
        // because it is rendered as "    ABC" on the screen.
        struct Coordinates
        {
            Coordinates() : Line(0), Column(0) {}
            Coordinates(int aLine, int aColumn) 
                : Line(aLine)
                , Column(aColumn)
            {
                assert(aLine >= 0);
                assert(aColumn >= 0);
            }
            static Coordinates Invalid() { static Coordinates invalid(-1, -1); return invalid; }

            bool operator ==(const Coordinates& o) const
            {
                return
                    Line == o.Line &&
                    Column == o.Column;
            }

            bool operator !=(const Coordinates& o) const
            {
                return
                    Line != o.Line ||
                    Column != o.Column;
            }

            bool operator <(const Coordinates& o) const
            {
                if (Line != o.Line)
                    return Line < o.Line;
                return Column < o.Column;
            }

            bool operator >(const Coordinates& o) const
            {
                if (Line != o.Line)
                    return Line > o.Line;
                return Column > o.Column;
            }

            bool operator <=(const Coordinates& o) const
            {
                if (Line != o.Line)
                    return Line < o.Line;
                return Column <= o.Column;
            }

            bool operator >=(const Coordinates& o) const
            {
                if (Line != o.Line)
                    return Line > o.Line;
                return Column >= o.Column;
            }

            int Line, Column;
        };

        struct Identifier
        {
            Coordinates Location;
            String Declaration;
        };

        typedef UnorderedMap<String, Identifier> Identifiers;
        typedef UnorderedSet<String> Keywords;
        typedef Map<int, String> ErrorMarkers;
        typedef UnorderedSet<int> Breakpoints;
        typedef std::array<ImU32, (unsigned)PaletteIndex::Max> Palette;
        typedef uint8_t Char;

        struct Glyph
        {
            Char Character;
            PaletteIndex ColorIndex = PaletteIndex::Default;
            bool Comment : 1;
            bool MultiLineComment : 1;
            bool Preprocessor : 1;

            Glyph(Char aChar, PaletteIndex aColorIndex) 
                : Character(aChar)
                , ColorIndex(aColorIndex)
                , Comment(false)
                , MultiLineComment(false)
                , Preprocessor(false) {}
        };

        typedef Vector<Glyph> Line;
        typedef Vector<Line> Lines;

        struct LanguageDefinition
        {
            typedef std::pair<String, PaletteIndex> TokenRegexString;
            typedef std::vector<TokenRegexString> TokenRegexStrings;
            typedef bool(*TokenizeCallback)(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, PaletteIndex& paletteIndex);
            
            String _name;
            Keywords _keywords;
            Identifiers _identifiers;
            Identifiers _preprocIdentifiers;
            String _commentStart, _commentEnd, _singleLineComment;
            char _preprocChar;
            bool _autoIndentation;

            TokenizeCallback _tokenize;
            TokenRegexStrings _tokenRegexStrings;
            bool _caseSensitive;

            LanguageDefinition()
                : _preprocChar('#')
                , _autoIndentation(true)
                , _tokenize(nullptr)
                , _caseSensitive(true)
            {
            }

            static const LanguageDefinition& CPlusPlus();
        };

    public:

        WidgetScript();
        ~WidgetScript();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

        void SetLanguageDefinition(const LanguageDefinition& languageDef);
        const LanguageDefinition& GetLanguageDefinition() const { return _languageDefinition; }

        const Palette& GetPalette() const { return _paletteBase; }
        void SetPalette(const Palette& aValue);

        void SetErrorMarkers(const ErrorMarkers& aMarkers) { _errorMarkers = aMarkers; }
        void SetBreakpoints(const Breakpoints& aMarkers) { _breakpoints = aMarkers; }

        String GetSelectedText() const;
        String GetCurrentLineText()const;

        int GetTotalLines() const { return (int)_lines.size(); }
        bool IsOverwrite() const { return _overwrite; }

        bool IsTextChanged() const { return _textChanged; }
        bool IsCursorPositionChanged() const { return _cursorPositionChanged; }

        bool IsColorizerEnabled() const { return _colorizerEnabled; }
        void SetColorizerEnable(bool value);

        Coordinates GetCursorPosition() const { return GetActualCursorCoordinates(); }
        void SetCursorPosition(const Coordinates& position);

        inline void SetHandleMouseInputs(bool value) { _handleMouseInputs = value; }
        inline bool IsHandleMouseInputsEnabled() const { return _handleKeyboardInputs; }

        inline void SetHandleKeyboardInputs(bool value) { _handleKeyboardInputs = value; }
        inline bool IsHandleKeyboardInputsEnabled() const { return _handleKeyboardInputs; }

        inline void SetImGuiChildIgnored(bool value) { _ignoreImGuiChild = value; }
        inline bool IsImGuiChildIgnored() const { return _ignoreImGuiChild; }

        inline void SetShowWhitespaces(bool value) { _showWhitespaces = value; }
        inline bool IsShowingWhitespaces() const { return _showWhitespaces; }

        void SetTabSize(int value);
        inline int GetTabSize() const { return _tabSize; }

        void InsertText(const String& value);
        void InsertText(const char* value);

        void MoveUp(int amount = 1, bool select = false);
        void MoveDown(int amount = 1, bool select = false);
        void MoveLeft(int amount = 1, bool select = false, bool wordMode = false);
        void MoveRight(int amount = 1, bool select = false, bool wordMode = false);
        void MoveTop(bool select = false);
        void MoveBottom(bool select = false);
        void MoveHome(bool select = false);
        void MoveEnd(bool select = false);

        void SetSelectionStart(const Coordinates& position);
        void SetSelectionEnd(const Coordinates& position);
        void SetSelection(const Coordinates& start, const Coordinates& end, SelectionMode mode = SelectionMode::Normal);
        void SelectWordUnderCursor();
        void SelectAll();
        bool HasSelection() const;

        void Copy();
        void Cut();
        void Paste();
        void Delete();

        bool CanUndo() const;
        bool CanRedo() const;
        void Undo(int steps = 1);
        void Redo(int steps = 1);

        static const Palette& GetDarkPalette();
        static const Palette& GetLightPalette();
        static const Palette& GetRetroBluePalette();

        void Render(const char* title, const ImVec2& size = ImVec2(), bool border = false);
        void SetText(const String& text);
        String GetText() const;

    private:
        typedef Vector<std::pair<std::regex, PaletteIndex>> RegexList;

        struct EditorState
        {
            Coordinates SelectionStart;
            Coordinates SelectionEnd;
            Coordinates CursorPosition;
        };

        class UndoRecord
        {
        public:
            UndoRecord() {}
            ~UndoRecord() {}

            // TODO
        };

        typedef std::vector<UndoRecord> UndoBuffer;

        Coordinates GetActualCursorCoordinates() const;
        Coordinates SanitizeCoordinates(const Coordinates& aValue) const;
        int GetLineMaxColumn(int line) const;

        // TODO

    private:
        void SaveAndCompile();
        void HandleKeyboardInputs();
        void HandleMouseInputs();
        void Render();

    private:
        Editor::SelectionData& _selections;
        String _currentScriptName;
        String _currentScriptPath;
        String _editorContent;
        bool _needsUpdateContent;
        bool _needsSave;

        float _lineSpacing;
        Lines _lines;
        EditorState _state;
        UndoBuffer _undoBuffer;
        int _undoIndex;

        int _tabSize;
        bool _overwrite;
        bool _readOnly;
        bool _withinRender;
        bool _scrollToCursor;
        bool _scrollToTop;
        bool _textChanged;
        bool _colorizerEnabled;
        float _textStart;                   // position (in pixels) where a code line starts relative to the left of the TextEditor.
        int  _leftMargin;
        bool _cursorPositionChanged;
        int _colorRangeMin;
        int _colorRangeMax;
        SelectionMode _selectionMode;
        bool _handleKeyboardInputs;
        bool _handleMouseInputs;
        bool _ignoreImGuiChild;
        bool _showWhitespaces;

        Palette _paletteBase;
        Palette _palette;
        LanguageDefinition _languageDefinition;
        RegexList _regexList;

        bool _checkComments;
        Breakpoints _breakpoints;
        ErrorMarkers _errorMarkers;
        ImVec2 _charAdvance;
        Coordinates _interactiveStart;
        Coordinates _interactiveEnd;
        String _lineBuffer;
        uint64_t _startTime;

        float _lastClick;
    };
}
