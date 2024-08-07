#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class TE_CORE_EXPORT StreamReader : public NonCopyable
    {
    public:
        virtual ~StreamReader() = 0;
        
        virtual bool IsStreamGood() const = 0;

        virtual UINT64 GetStreamPosition() const = 0;

        virtual void SetStreamPosition(UINT64 position) = 0;

        virtual bool ReadData(UINT8* dest, size_t size) = 0;

        operator bool() const { return IsStreamGood(); }

        void ReadBuffer(UINT8* buffer, size_t size);

        void ReadString(std::string& string);

        template<typename T>
        void ReadRaw(T& value)
        {
            bool success = ReadData(reinterpret_cast<UINT8*>(&value), sizeof(T));
            TE_ASSERT_ERROR_SHORT(success);
        }

        template<typename T>
        void ReadObject(T& obj)
        {
            T::Deserialize(this, obj);
        }

        template<typename Key, typename Value>
        void ReadMap(Map<Key, Value>& map)
        {
            UINT32 size;
            ReadRaw<UINT32>(size);

            for (UINT32 i = 0; i < size; i++)
            {
                Key key;
                if constexpr (std::is_trivial<Key>())
                    ReadRaw<Key>(key);
                else
                    ReadObject<Key>(key);

                if constexpr (std::is_trivial<Value>())
                    ReadRaw<Value>(map[key]);
                else
                    ReadObject<Value>(map[key]);
            }
        }

        template<typename Key, typename Value>
        void ReadMap(UnorderedMap<Key, Value>& map)
        {
            UINT32 size;
            ReadRaw<UINT32>(size);

            for (UINT32 i = 0; i < size; i++)
            {
                Key key;
                if constexpr (std::is_trivial<Key>())
                    ReadRaw<Key>(key);
                else
                    ReadObject<Key>(key);

                if constexpr (std::is_trivial<Value>())
                    ReadRaw<Value>(map[key]);
                else
                    ReadObject<Value>(map[key]);
            }
        }

        template<typename Value>
        void ReadMap(const Map<String, Value>& map)
        {
            UINT32 size;
            ReadRaw<UINT32>(size);

            for (UINT32 i = 0; i < size; i++)
            {
                String key;
                ReadString(key);

                if constexpr (std::is_trivial<Value>())
                    ReadRaw<Value>(map[key]);
                else
                    ReadObject<Value>(map[key]);
            }
        }

        template<typename Value>
        void ReadMap(UnorderedMap<String, Value>& map)
        {
            UINT32 size;
            ReadRaw<UINT32>(size);

            for (UINT32 i = 0; i < size; i++)
            {
                String key;
                ReadString(key);

                if constexpr (std::is_trivial<Value>())
                    ReadRaw<Value>(map[key]);
                else
                    ReadObject<Value>(map[key]);
            }
        }

        template<typename T>
        void ReadArray(Vector<T>& array)
        {
            UINT32 size;
            ReadRaw<UINT32>(size);

            array.resize(size);

            for (UINT32 i = 0; i < size; i++)
            {
                if constexpr (std::is_trivial<T>())
                    ReadRaw<T>(array[i]);
                else
                    ReadObject<T>(array[i]);
            }
        }

        void ReadArray(Vector<String>& array)
        {
            UINT32 size;
            ReadRaw<UINT32>(size);

            array.resize(size);

            for (UINT32 i = 0; i < size; i++)
            {
                ReadString(array[i]);
            }
        }
    };
}
