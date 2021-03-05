#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"
#include "ImGui/imgui.h"

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

        typedef UnorderedMap<std::string, Identifier> Identifiers;
        typedef UnorderedSet<std::string> Keywords;
        typedef Map<int, std::string> ErrorMarkers;
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
            typedef std::pair<std::string, PaletteIndex> TokenRegexString;
            typedef std::vector<TokenRegexString> TokenRegexStrings;
            typedef bool(*TokenizeCallback)(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, PaletteIndex& paletteIndex);
            
            String _name;
            Keywords _keywords;
            Identifiers _identifiers;
            Identifiers _preprocIdentifiers;
            std::string _commentStart, _commentEnd, _singleLineComment;
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

    private:
        void SaveAndCompile();

    private:
        Editor::SelectionData& _selections;
        String _currentScriptName;
        String _currentScriptPath;
        String _editorContent;
        bool _needsUpdateContent;
        bool _needsSave;
    };
}
