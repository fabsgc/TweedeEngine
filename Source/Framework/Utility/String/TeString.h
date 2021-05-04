#pragma once

#include "Prerequisites/TePlatformDefines.h"
#include "Prerequisites/TeTypes.h"

#include <string>
#include <sstream>
#include <regex>

namespace te
{
    template <typename T>
    using BasicString = std::basic_string <T, std::char_traits<T>>;

    /** \brief Basic string stream that uses Banshee memory allocators. */
    template <typename T>
    using BasicStringStream = std::basic_stringstream <T, std::char_traits<T>>;

    /** \brief Wide string used primarily for handling Unicode text (UTF-32 on Linux, UTF-16 on Windows, generally). */
    typedef BasicString<wchar_t> WString;

    /** \brief Narrow string used for handling narrow encoded text (either locale specific ANSI or UTF-8). */
    typedef BasicString<char> String;

    /** \brief IStringStream wrapper */
    template <typename T>
    using BasicIStringStream = std::basic_istringstream<T, std::char_traits<T>>;

    typedef BasicIStringStream<char> IStringStream;
    typedef BasicIStringStream<wchar_t> IWStringStream;

    /** \brief Wide string used UTF-16 encoded strings. */
    typedef BasicString<char16_t> U16String;

    /** \brief Wide string used UTF-32 encoded strings. */
    typedef BasicString<char32_t> U32String;

    /** \brief Wide string stream used for primarily for constructing wide strings. */
    typedef BasicStringStream<wchar_t> WStringStream;

    /** \brief Wide string stream used for primarily for constructing narrow strings. */
    typedef BasicStringStream<char> StringStream;

    /** \brief Wide string stream used for primarily for constructing UTF-16 strings. */
    typedef BasicStringStream<char16_t> U16StringStream;

    /** \brief Wide string stream used for primarily for constructing UTF-32 strings. */
    typedef BasicStringStream<char32_t> U32StringStream;

    /* ###################################################################
    *  ############# STRING CONVERSION FUNCTIONS #########################
    *  ################################################################ */

    /** \brief Converts a narrow string to a wide string. */
    TE_UTILITY_EXPORT WString ToWString(const String& source);

    /** \brief Converts a narrow string to a wide string. */
    TE_UTILITY_EXPORT WString ToWString(const char* source);

    /** \brief Converts a float to a string. */
    TE_UTILITY_EXPORT WString ToWString(float val,
        unsigned short precision = 6,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts a double to a string. */
    TE_UTILITY_EXPORT WString ToWString(double val,
        unsigned short precision = 6, unsigned short width = 0,
        char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an int to a string. */
    TE_UTILITY_EXPORT WString ToWString(int val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an unsigned int to a string. */
    TE_UTILITY_EXPORT WString ToWString(unsigned int val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an 64bit integer to a string. */
    TE_UTILITY_EXPORT WString ToWString(INT64 val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an 64bit unsigned to a string. */
    TE_UTILITY_EXPORT WString ToWString(UINT64 val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an narrow char unsigned to a string. */
    TE_UTILITY_EXPORT WString ToWString(char val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an wide bit char unsigned to a string. */
    TE_UTILITY_EXPORT WString ToWString(wchar_t val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts a wide string to a narrow string. */
    TE_UTILITY_EXPORT String ToString(const WString& source);

    /** \brief Converts a wide string to a narrow string. */
    TE_UTILITY_EXPORT String ToString(const wchar_t* source);

    /** \brief Converts a float to a string. */
    TE_UTILITY_EXPORT String ToString(float val,
        unsigned short precision = 6, unsigned short width = 0,
        char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts a double to a string. */
    TE_UTILITY_EXPORT String ToString(double val,
        unsigned short precision = 6, unsigned short width = 0,
        char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

    TE_UTILITY_EXPORT String ToString(int val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an unsigned int to a string. */
    TE_UTILITY_EXPORT String ToString(unsigned int val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts a 64bit int to a string. */
    TE_UTILITY_EXPORT String ToString(INT64 val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /** \brief Converts an 64bit unsigned int to a string. */
    TE_UTILITY_EXPORT String ToString(UINT64 val,
        unsigned short width = 0, char fill = ' ',
        std::ios::fmtflags flags = std::ios::fmtflags(0));

    /**
    * \brief Converts a String to a float.
    * \note	0.0f if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT float ParseFloat(const String& val, float defaultValue = 0);

    /**
    * \brief Converts a String to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT INT32 ParseINT32(const String& val, INT32 defaultValue = 0);

    /**
    * \brief Converts a String to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT UINT32 ParseUINT32(const String& val, UINT32 defaultValue = 0);

    /**
    * \brief Converts a String to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT INT64 ParseINT64(const String& val, INT64 defaultValue = 0);

    /**
    * \brief Converts a String to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT UINT64 ParseUINT64(const String& val, UINT64 defaultValue = 0);

    /** \brief Checks the String is a valid number value. */
    TE_UTILITY_EXPORT bool IsNumber(const String& val);

    /**
    * \brief Converts a WString to a float.
    * \note	0.0f if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT float ParseFloat(const WString& val, float defaultValue = 0);

    /**
    * \brief Converts a WString to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT INT32 ParseINT32(const WString& val, INT32 defaultValue = 0);

    /**
    * \brief Converts a WString to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT UINT32 ParseUINT32(const WString& val, UINT32 defaultValue = 0);

    /**
    * \brief Converts a WString to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT INT64 ParseINT64(const WString& val, INT64 defaultValue = 0);

    /**
    * \brief Converts a WString to a whole number.
    * \note	0 if the value could not be parsed, otherwise the numeric version of the string.
    */
    TE_UTILITY_EXPORT UINT64 ParseUINT64(const WString& val, UINT64 defaultValue = 0);

    /**
    * \brief Checks the WString is a valid number value.
    */
    TE_UTILITY_EXPORT bool IsNumber(const WString& val);

    /**
    * \brief Split string according to delimiter
    */
    TE_UTILITY_EXPORT Vector<String> Split(const String& s, char delimiter);

    /**
    * \brief Split string according to delimiter
    */
    TE_UTILITY_EXPORT Vector<String> Split(const WString& s, char delimiter);

    /** Converts all the characters in the string to lower case. Does not handle UTF8 encoded strings. */
    TE_UTILITY_EXPORT void ToLowerCase(String& str);

    /** Converts all the characters in the string to lower case. Does not handle UTF8 encoded strings. */
    TE_UTILITY_EXPORT void ToLowerCase(WString& str);

    /** Converts all the characters in the string to upper case. Does not handle UTF8 encoded strings. */
    TE_UTILITY_EXPORT void ToUpperCase(String& str);

    /** Converts all the characters in the string to upper case. Does not handle UTF8 encoded strings. */
    TE_UTILITY_EXPORT void ToUpperCase(WString& str);

    /**
    * Returns whether the string begins with the pattern passed in.
    *
    * @param[in]	str		 	String to compare.
    * @param[in]	pattern		Pattern to compare with.
    * @param[in]	lowerCase	(optional) If true, the start of the string will be lower cased before comparison, and
    *							the pattern should also be in lower case.
    */
    TE_UTILITY_EXPORT bool StartsWith(const String& str, const String& pattern, bool lowerCase = true);

    /** @copydoc StartsWith(const String&, const String&, bool) */
    TE_UTILITY_EXPORT bool StartsWith(const WString& str, const WString& pattern, bool lowerCase = true);

    /**
     * Returns whether the string end with the pattern passed in.
     *
     * @param[in]	str		 	String to compare.
     * @param[in]	pattern		Pattern to compare with.
     * @param[in]	lowerCase	(optional) If true, the start of the string will be lower cased before comparison, and
     *							the pattern should also be in lower case.
     */
    TE_UTILITY_EXPORT bool EndsWith(const String& str, const String& pattern, bool lowerCase = true);

    /** @copydoc EndsWith(const String&, const String&, bool) */
    TE_UTILITY_EXPORT  bool EndsWith(const WString& str, const WString& pattern, bool lowerCase = true);

    /**
     * Returns true if the string matches the provided pattern. Pattern may use a "*" wildcard for matching any
     * characters.
     *
     * @param[in]	str			 	The string to test.
     * @param[in]	pattern		 	Patterns to look for.
     * @param[in]	caseSensitive	(optional) Should the match be case sensitive or not.
     */
    TE_UTILITY_EXPORT  bool Match(const String& str, const String& pattern, bool caseSensitive = true);

    /** @copydoc Match(const String&, const String&, bool) */
    TE_UTILITY_EXPORT  bool Match(const WString& str, const WString& pattern, bool caseSensitive = true);

    /**
     * Returns true if the string matches the provided regex pattern.
     *
     * @param[in]	str			 	The string to test.
     * @param[in]	pattern		 	Patterns to look for.
     * @param[in]	caseSensitive	(optional) Should the match be case sensitive or not.
     */
    TE_UTILITY_EXPORT  bool RegexMatch(const String& str, const String& pattern, bool caseSensitive = true);

    /** @copydoc RegexMatch(const String&, const String&, bool) */
    TE_UTILITY_EXPORT  bool RegexMatch(const WString& str, const WString& pattern, bool caseSensitive = true);

    /**
    * Replace all instances of a substring with a another substring.
    *
    * @param[in]	source		   	String to search.
    * @param[in]	replaceWhat	   	Substring to find and replace
    * @param[in]	replaceWithWhat	Substring to replace with (the new sub-string)
    *
    * @return	An updated string with the substrings replaced.
    */
    TE_UTILITY_EXPORT const String ReplaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat);

    /** @copydoc ReplaceAll(const String&, const String&, const String&) */
    TE_UTILITY_EXPORT const WString ReplaceAll(const WString& source, const WString& replaceWhat, const WString& replaceWithWhat);

    template <class T>
    static bool StartsWithInternal(const BasicString<T>& str, const BasicString<T>& pattern, bool lowerCase)
    {
        size_t thisLen = str.length();
        size_t patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        BasicString<T> startOfThis = str.substr(0, patternLen);
        if (lowerCase)
            Util::ToLowerCase(startOfThis);

        return (startOfThis == pattern);
    }

    template <class T>
    static bool EndsWithInternal(const BasicString<T>& str, const BasicString<T>& pattern, bool lowerCase)
    {
        size_t thisLen = str.length();
        size_t patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        BasicString<T> endOfThis = str.substr(thisLen - patternLen, patternLen);
        if (lowerCase)
            Util::ToLowerCase(endOfThis);

        return (endOfThis == pattern);
    }

    template <class T>
    static bool RegexMatchInternal(const BasicString<T>& str, const BasicString<T>& pattern, bool caseSensitive)
    {
        BasicString<T> tmpStr = str;
        std::basic_regex<T> tmpPattern(pattern);
        if (!caseSensitive)
            Util::ToLowerCase(tmpStr);

        return std::regex_match(tmpStr, tmpPattern);
    }

    template <class T>
    static bool MatchInternal(const BasicString<T>& str, const BasicString<T>& pattern, bool caseSensitive)
    {
        BasicString<T> tmpStr = str;
        BasicString<T> tmpPattern = pattern;
        if (!caseSensitive)
        {
            Util::ToLowerCase(tmpStr);
            Util::ToLowerCase(tmpPattern);
        }

        typename BasicString<T>::const_iterator strIt = tmpStr.begin();
        typename BasicString<T>::const_iterator patIt = tmpPattern.begin();
        typename BasicString<T>::const_iterator lastWildCardIt = tmpPattern.end();
        while (strIt != tmpStr.end() && patIt != tmpPattern.end())
        {
            if (*patIt == '*')
            {
                lastWildCardIt = patIt;
                // Skip over looking for next character
                ++patIt;
                if (patIt == tmpPattern.end())
                {
                    // Skip right to the end since * matches the entire rest of the string
                    strIt = tmpStr.end();
                }
                else
                {
                    // scan until we find next pattern character
                    while (strIt != tmpStr.end() && *strIt != *patIt)
                        ++strIt;
                }
            }
            else
            {
                if (*patIt != *strIt)
                {
                    if (lastWildCardIt != tmpPattern.end())
                    {
                        // The last wildcard can match this incorrect sequence
                        // rewind pattern to wildcard and keep searching
                        patIt = lastWildCardIt;
                        lastWildCardIt = tmpPattern.end();
                    }
                    else
                    {
                        // no wildwards left
                        return false;
                    }
                }
                else
                {
                    ++patIt;
                    ++strIt;
                }
            }
        }

        // If we reached the end of both the pattern and the string, we succeeded
        if (patIt == tmpPattern.end() && strIt == tmpStr.end())
            return true;
        else
            return false;
    }

    template <class T>
    BasicString<T> ReplaceAllInternal(const BasicString<T>& source,
        const BasicString<T>& replaceWhat, const BasicString<T>& replaceWithWhat)
    {
        BasicString<T> result = source;
        typename BasicString<T>::size_type pos = 0;
        while (1)
        {
            pos = result.find(replaceWhat, pos);
            if (pos == BasicString<T>::npos) break;
            result.replace(pos, replaceWhat.size(), replaceWithWhat);
            pos += replaceWithWhat.size();
        }
        return result;
    }
}
