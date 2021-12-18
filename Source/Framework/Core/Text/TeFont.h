#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "TeFontDesc.h"

namespace te
{
    /**	Contains textures and data about every character for a bitmap font of a specific size. */
    struct TE_CORE_EXPORT FontBitmap
    {
        /**	Returns a character description for the character with the specified Unicode key. */
        const CharDesc& GetCharDesc(UINT32 charId) const;

        /** Font size for which the data is contained. */
        UINT32 Size;

        /** Y offset to the baseline on which the characters are placed, in pixels. */
        INT32 BaselineOffset;

        /** Height of a single line of the font, in pixels. */
        UINT32 LineHeight;

        /** Character to use when data for a character is missing. */
        CharDesc MissingGlyph;

        /** Width of a space in pixels. */
        UINT32 SpaceWidth;

        /** Textures in which the character's pixels are stored. */
        Vector<HTexture> TexturePages;

        /** All characters in the font referenced by character ID. */
        UnorderedMap<UINT32, CharDesc> Characters;
    };

    /**
     * Font resource containing data about textual characters and how to render text. Contains one or multiple font
     * bitmaps, each for a specific size.
     */
    class TE_CORE_EXPORT Font : public Resource
    {
    public:
        virtual ~Font() = default;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TypeID_Core::TID_Font; }

        /**
         * Initializes the font with specified per-size font data.
         * @note	Internal method. Factory methods will call this automatically for you.
         */
        void Initialize(const Vector<SPtr<FontBitmap>>& fontData);

        /**	Creates a new font from the provided per-size font data. */
        static HFont Create(const Vector<SPtr<FontBitmap>>& fontInitData);

        /** Creates a new font as a pointer instead of a resource handle. */
        static SPtr<Font> CreatePtr(const Vector<SPtr<FontBitmap>>& fontInitData);

        /** Creates a Font without initializing it. */
        static SPtr<Font> CreateEmpty();

        // TODO

    protected:
        friend class FontManager;

        Font();

    private:
        UnorderedMap<UINT32, SPtr<FontBitmap>> _fontDataPerSize;
    };
}
