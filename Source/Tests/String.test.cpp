#include <gtest/gtest.h>

#include "String/TeString.h"

TEST(StringConversion, ConvertsAsciiWideString)
{
    EXPECT_EQ(te::ToString(te::WString(L"Hello, world!")), "Hello, world!");
}

TEST(StringConversion, ConvertsUnicodeWideStringToUtf8)
{
    const te::WString source = L"\u00E9\u6C34\U0001F600";
    const te::String expected = "\xC3\xA9\xE6\xB0\xB4\xF0\x9F\x98\x80";

    EXPECT_EQ(te::ToString(source), expected);
}

TEST(StringConversion, ConvertsEmptyWideString)
{
    EXPECT_TRUE(te::ToString(te::WString()).empty());
}

TEST(StringConversion, PreservesEmbeddedNullCharacters)
{
    const te::WString source{ L'A', L'\0', L'B' };
    const te::String expected{ 'A', '\0', 'B' };

    EXPECT_EQ(te::ToString(source), expected);
}
