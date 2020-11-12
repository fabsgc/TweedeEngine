#pragma once

#include "String/TeString.h"

namespace te
{
    /** Generates a new hash for the provided type using the default standard hasher and combines it with a previous hash. */
    template <class T>
    void te_hash_combine(std::size_t& seed, const T& v)
    {
        using HashType = typename std::conditional<std::is_enum<T>::value, EnumClassHash, std::hash<T>>::type;

        HashType hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    /** Generates a hash for the provided type. Type must have a std::hash specialization. */
    template <class T>
    size_t te_hash(const T& v)
    {
        using HashType = typename std::conditional<std::is_enum<T>::value, EnumClassHash, std::hash<T>>::type;

        HashType hasher;
        return hasher(v);
    }

    class TE_UTILITY_EXPORT Util
    {
    public:
        /** Get the extension of a file (with ".") */
        static String GetFileExtension(const String& filePath);

        /** Generates an MD5 hash string for the provided source string. */
        String Md5(const WString& source);

        /**	Generates an MD5 hash string for the provided source string. */
        String Md5(const String& source);

        /** Get current time and date into a char array */
        void GetTime(char* buffer);

        /**
         * Returns a vector of strings containing all the substrings delimited by the provided delimiter characters.
         *
         * @param[in]	str		 	The string to split.
         * @param[in]	delims   	(optional) Delimiter characters to split the string by. They will not
         * 							be included in resulting substrings.
         * @param[in]	maxSplits	(optional) The maximum number of splits to perform (0 for unlimited splits). If this
         *							parameters is > 0, the splitting process will stop after this many splits, left to right.
         */
        static Vector<String> Split(const String& str, const String& delims = "\t\n ", UINT16 maxSplits = 0);

        /** @copydoc StringUtil::split(const String&, const String&, unsigned int) */
        static Vector<WString> Split(const WString& str, const WString& delims = L"\t\n ", UINT16 maxSplits = 0);

        /** Removes any whitespace characters from beginning or end of the string. */
        static void Trim(String& str, bool left = true, bool right = true);

        /** Removes any whitespace characters from beginning or end of the string. */
        static void Trim(WString& str, bool left = true, bool right = true);

        /** Removes any whitespace characters from beginning or end of the string. */
        static void Trim(String& str, const String& delims, bool left = true, bool right = true);

        /** Removes any whitespace characters from beginning or end of the string. */
        static void Trim(WString& str, const WString& delims, bool left = true, bool right = true);

        /** Converts all the characters in the string to lower case. Does not handle UTF8 encoded strings. */
        static void ToLowerCase(String& str);

        /** Converts all the characters in the string to lower case. Does not handle UTF8 encoded strings. */
        static void ToLowerCase(WString& str);

        /** Converts all the characters in the string to upper case. Does not handle UTF8 encoded strings. */
        static void ToUpperCase(String& str);

        /**	Converts all the characters in the string to upper case. Does not handle UTF8 encoded strings. */
        static void ToUpperCase(WString& str);

        template <class T>
        static Vector<BasicString<T>> SplitInternal(const BasicString<T>& str, const BasicString<T>& delims, UINT16 maxSplits)
        {
            Vector<BasicString<T>> ret;
            // Pre-allocate some space for performance
            ret.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

            unsigned int numSplits = 0;

            // Use STL methods
            size_t start, pos;
            start = 0;
            do
            {
                pos = str.find_first_of(delims, start);
                if (pos == start)
                {
                    // Do nothing
                    start = pos + 1;
                }
                else if (pos == BasicString<T>::npos || (maxSplits && numSplits == maxSplits))
                {
                    // Copy the rest of the string
                    ret.push_back(str.substr(start));
                    break;
                }
                else
                {
                    // Copy up to delimiter
                    ret.push_back(str.substr(start, pos - start));
                    start = pos + 1;
                }
                // parse up to next real data
                start = str.find_first_not_of(delims, start);
                ++numSplits;

            } while (pos != BasicString<T>::npos);

            return ret;
        }

        static bool IsBigEndian();
    };

    /** Sets contents of a struct to zero. */
    template<class T>
    void te_zero_out(T& s)
    {
        std::memset(&s, 0, sizeof(T));
    }

    /** Sets contents of a static array to zero. */
    template<class T, size_t N>
    void te_zero_out(T(&arr)[N])
    {
        std::memset(arr, 0, sizeof(T) * N);
    }

    /** Sets contents of a block of memory to zero. */
    template<class T>
    void te_zero_out(T* arr, size_t count)
    {
        assert(arr != nullptr);
        std::memset(arr, 0, sizeof(T) * count);
    }

    /** Copies the contents of one array to another. Automatically accounts for array element size. */
    template<class T, size_t N>
    void te_copy(T(&dst)[N], T(&src)[N], size_t count)
    {
        std::memcpy(dst, src, sizeof(T) * count);
    }

    /** Copies the contents of one array to another. Automatically accounts for array element size. */
    template<class T>
    void te_copy(T* dst, T* src, size_t count)
    {
        std::memcpy(dst, src, sizeof(T) * count);
    }

    /** Returns the size of the provided static array. */
    template <class T, std::size_t N>
    constexpr size_t te_size(const T(&array)[N])
    {
        return N;
    }
}
