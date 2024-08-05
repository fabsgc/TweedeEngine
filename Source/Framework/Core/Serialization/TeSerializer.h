#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class TE_CORE_EXPORT Serializer : public NonCopyable
    {
    public:
        virtual ~Serializer() = 0;
        
        virtual bool IsStreamGood() const = 0;
        virtual UINT64 GetStreamPosition() const = 0;
        virtual void SetStreamPosition(UINT64 position) = 0;
        virtual bool WriteData(const UINT8* data, size_t size) = 0;

        operator bool() const { return IsStreamGood(); }

        void WriteBuffer(UINT8* buffer, size_t size);
        void WriteZero(uint64_t size);
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
    };
}
