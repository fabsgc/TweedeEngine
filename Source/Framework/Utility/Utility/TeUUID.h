#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
    /** Represents a universally unique identifier. */
    struct TE_UTILITY_EXPORT UUID
    {
        /** Initializes an empty UUID. */
        constexpr UUID() = default;

        /** Initializes an UUID using Banshee's UUID representation. */
        constexpr UUID(UINT32 data1, UINT32 data2, UINT32 data3, UINT32 data4)
            : _data{ data1, data2, data3, data4 }
        { }

        /** Initializes an UUID using its string representation. */
        explicit UUID(const String& uuid);

        constexpr bool operator==(const UUID& rhs) const
        {
            return _data[0] == rhs._data[0] && _data[1] == rhs._data[1] && _data[2] == rhs._data[2] && _data[3] == rhs._data[3];
        }

        constexpr bool operator!=(const UUID& rhs) const
        {
            return !(*this == rhs);
        }

        constexpr bool operator<(const UUID& rhs) const
        {
            for (UINT32 i = 0; i < 4; i++)
            {
                if (_data[i] < rhs._data[i])
                    return true;
                else if (_data[i] > rhs._data[i])
                    return false;

                // Move to next element if equal
            }

            // They're equal
            return false;
        }

        /** Checks has the UUID been initialized to a valid value. */
        constexpr bool Empty() const
        {
            return _data[0] == 0 && _data[1] == 0 && _data[2] == 0 && _data[3] == 0;
        }

        /** Converts the UUID into its string representation. */
        String ToString() const;

        static UUID EMPTY;
    private:
        friend struct std::hash<UUID>;

        UINT32 _data[4] = { 0, 0, 0, 0 };
    };

    /**
     * Utility class for generating universally unique identifiers.
     *
     * @note	Thread safe.
     */
    class TE_UTILITY_EXPORT UUIDGenerator
    {
    public:
        /**	Generate a new random universally unique identifier. */
        static UUID GenerateRandom();
    };
}

namespace std
{
    /**	Hash value generator for UUID. */
    template<>
    struct hash<te::UUID>
    {
        size_t operator()(const te::UUID& value) const
        {
            size_t hash = 0;
            te::hash_combine(hash, value._data[0]);
            te::hash_combine(hash, value._data[1]);
            te::hash_combine(hash, value._data[2]);
            te::hash_combine(hash, value._data[3]);

            return hash;
        }
    };
}