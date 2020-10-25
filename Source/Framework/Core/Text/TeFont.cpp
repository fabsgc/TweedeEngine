#include "Text/TeFont.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    const CharDesc& FontBitmap::GetCharDesc(UINT32 charId) const
    {
        auto iterFind = Characters.find(charId);
        if (iterFind != Characters.end())
        {
            return Characters.at(charId);
        }

        return MissingGlyph;
    }

    Font::Font()
        : Resource(TID_Font)
    { }

    void Font::Initialize(const Vector<SPtr<FontBitmap>>& fontData)
    { 
        for (auto iter = fontData.begin(); iter != fontData.end(); ++iter)
        {
            _fontDataPerSize[(*iter)->Size] = *iter;
        }

        Resource::Initialize();
    }

    HFont Font::Create(const Vector<SPtr<FontBitmap>>& fontData)
    {
        SPtr<Font> newFont = _createPtr(fontData);
        return static_resource_cast<Font>(gResourceManager()._createResourceHandle(newFont));
    }

    SPtr<Font> Font::_createPtr(const Vector<SPtr<FontBitmap>>& fontData)
    {
        SPtr<Font> newFont = te_core_ptr<Font>(new (te_allocate<Font>()) Font());
        newFont->SetThisPtr(newFont);
        newFont->Initialize(fontData);

        return newFont;
    }

    SPtr<Font> Font::_createEmpty()
    {
        SPtr<Font> newFont = te_core_ptr<Font>(new (te_allocate<Font>()) Font());
        newFont->SetThisPtr(newFont);

        return newFont;
    }
}
