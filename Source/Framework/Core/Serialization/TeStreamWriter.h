#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class TE_CORE_EXPORT StreamWriter : public NonCopyable
    {
    public:
        virtual ~StreamWriter() = 0;
        
        virtual bool IsStreamGood() const = 0;

        virtual UINT64 GetStreamPosition() const = 0;
        
        virtual void SetStreamPosition(UINT64 position) = 0;

        operator bool() const { return IsStreamGood(); }

        void WriteZero(uint64_t size);

        void WriteBuffer(const UINT8* buffer, size_t size);

        void WriteString(const std::string& string);

        template<typename T>
        void WriteRaw(const T& value)
        {
            bool success = WriteData(reinterpret_cast<const UINT8*>(&value), sizeof(T));
            TE_ASSERT_ERROR_SHORT(success);
        }

        template<typename T>
        void WriteObject(const T& obj)
        {
            obj.Serialize(this);
        }

        template<typename Key, typename Value>
        void WriteMap(const Map<Key, Value>& map)
        {
            WriteRaw<uint32_t>((uint32_t)map.size());

            for (const auto& [key, value] : map)
            {
                if constexpr (std::is_trivial<Key>())
                    WriteRaw<Key>(key);
                else
                    WriteObject<Key>(key);

                if constexpr (std::is_trivial<Value>())
                    WriteRaw<Value>(value);
                else
                    WriteObject<Value>(value);
            }
        }

        template<typename Key, typename Value>
        void WriteMap(const UnorderedMap<Key, Value>& map)
        {
            WriteRaw<uint32_t>((uint32_t)map.size());

            for (const auto& [key, value] : map)
            {
                if constexpr (std::is_trivial<Key>())
                    WriteRaw<Key>(key);
                else
                    WriteObject<Key>(key);

                if constexpr (std::is_trivial<Value>())
                    WriteRaw<Value>(value);
                else
                    WriteObject<Value>(value);
            }
        }

        template<typename Value>
        void WriteMap(const Map<String, Value>& map)
        {
            WriteRaw<uint32_t>((uint32_t)map.size());

            for (const auto& [key, value] : map)
            {
                WriteString(key);

                if constexpr (std::is_trivial<Value>())
                    WriteRaw<Value>(value);
                else
                    WriteObject<Value>(value);
            }
        }

        template<typename Value>
        void WriteMap(const UnorderedMap<String, Value>& map)
        {
            WriteRaw<uint32_t>((uint32_t)map.size());

            for (const auto& [key, value] : map)
            {
                WriteString(key);

                if constexpr (std::is_trivial<Value>())
                    WriteRaw<Value>(value);
                else
                    WriteObject<Value>(value);
            }
        }

        template<typename T>
        void WriteArray(const std::vector<T>& array)
        {
            WriteRaw<uint32_t>((uint32_t)array.size());

            for (const auto& element : array)
            {
                if constexpr (std::is_trivial<T>())
                    WriteRaw<T>(element);
                else
                    WriteObject<T>(element);
            }
        }

        void WriteArray(const std::vector<String>& array)
        {
            WriteRaw<uint32_t>((uint32_t)array.size());

            for (const auto& element : array)
            {
                WriteString(element);
            }
        }

    protected:
        virtual bool WriteData(const UINT8* data, size_t size) = 0;
    };
}
