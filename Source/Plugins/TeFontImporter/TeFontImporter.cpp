#include "TeFontImporter.h"
#include "Text/TeFontImportOptions.h"
#include "Image/TePixelData.h"
#include "Image/TeTexture.h"
#include "Utility/TeFileSystem.h"

#include <filesystem>
#include <ft2build.h>
#include <freetype/freetype.h>
#include FT_FREETYPE_H

using namespace std::placeholders;

namespace te
{ 
    FontImporter::FontImporter()
        : BaseImporter()
    {
        _extensions.push_back(u8"ttf");
        _extensions.push_back(u8"otf");
    }

    bool FontImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), tolower);
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> FontImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<FontImportOptions>();
    }

    SPtr<Resource> FontImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        const FontImportOptions* fontImportOptions = static_cast<const FontImportOptions*>(importOptions.get());

        FT_Library library;

        FT_Error error = FT_Init_FreeType(&library);
        if (error)
            TE_ASSERT_ERROR(false, "Error occurred during FreeType library initialization.");

        FT_Face face;
        {
            Lock lock = FileScheduler::GetLock(filePath);
            error = FT_New_Face(library, filePath.c_str(), 0, &face);
        }

        if (error == FT_Err_Unknown_File_Format)
        {
            TE_ASSERT_ERROR(false, "Failed to load font file: " + filePath + ". Unsupported file format.");
        }
        else if (error)
        {
            TE_ASSERT_ERROR(false, "Failed to load font file: " + filePath + ". Unknown error.");
        }

        Vector<CharRange> charIndexRanges = fontImportOptions->CharIndexRanges;
        Vector<UINT32> fontSizes = fontImportOptions->FontSizes;
        UINT32 dpi = fontImportOptions->Dpi;

        FT_Int32 loadFlags;
        switch (fontImportOptions->RenderMode)
        {
        case FontRenderMode::Smooth:
            loadFlags = FT_LOAD_TARGET_NORMAL | FT_LOAD_NO_HINTING;
            break;
        case FontRenderMode::Raster:
            loadFlags = FT_LOAD_TARGET_MONO | FT_LOAD_NO_HINTING;
            break;
        case FontRenderMode::HintedSmooth:
            loadFlags = FT_LOAD_TARGET_NORMAL | FT_LOAD_NO_AUTOHINT;
            break;
        case FontRenderMode::HintedRaster:
            loadFlags = FT_LOAD_TARGET_MONO | FT_LOAD_NO_AUTOHINT;
            break;
        default:
            loadFlags = FT_LOAD_TARGET_NORMAL;
            break;
        }

        FT_Render_Mode renderMode = FT_LOAD_TARGET_MODE(loadFlags);

        Vector<SPtr<FontBitmap>> dataPerSize;
        for (size_t i = 0; i < fontSizes.size(); i++)
        { }

        FT_Done_FreeType(library);

        SPtr<Font> newFont = Font::_createPtr(dataPerSize);
        newFont->SetName(std::filesystem::path(filePath).filename().generic_string());
        newFont->SetPath(filePath);
        return newFont;
    }
}
