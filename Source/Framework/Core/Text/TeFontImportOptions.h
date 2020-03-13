#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"
#include "Text/TeFont.h"

namespace te
{
    /**	Determines how is a font rendered into the bitmap texture. */
    enum class TE_CORE_EXPORT FontRenderMode
    {
        Smooth, /*< Render antialiased fonts without hinting (slightly more blurry). */
        Raster, /*< Render non-antialiased fonts without hinting (slightly more blurry). */
        HintedSmooth, /*< Render antialiased fonts with hinting. */
        HintedRaster /*< Render non-antialiased fonts with hinting. */
    };

    /** Represents a range of character code. */
    struct TE_CORE_EXPORT CharRange
    {
        CharRange() = default;
        CharRange(UINT32 start, UINT32 end)
            : Start(start), End(end)
        { }

        UINT32 Start = 0;
        UINT32 End = 0;
    };

    /**	Import options that allow you to control how is a font imported. */
    class TE_CORE_EXPORT FontImportOptions : public ImportOptions
    {
    public:
        FontImportOptions() = default;

        /**	Determines font sizes that are to be imported. Sizes are in points. */
        Vector<UINT32> FontSizes = { 10 };

        /**	Determines character index ranges to import. Ranges are defined as unicode numbers. */
        Vector<CharRange> CharIndexRanges = { CharRange(33, 166) }; // Most used ASCII characters

        /**	Determines dots per inch scale that will be used when rendering the characters. */
        UINT32 Dpi = 96;

        /**	Determines the render mode used for rendering the characters into a bitmap. */
        FontRenderMode RenderMode = FontRenderMode::HintedSmooth;

        /**	Determines whether the bold font style should be used when rendering. */
        bool Bold = false;

        /**	Determines whether the italic font style should be used when rendering. */
        bool Italic = false;

        /** Creates a new import options object that allows you to customize how are fonts imported. */
        static SPtr<FontImportOptions> Create();
    };
}
