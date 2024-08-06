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

    protected:
        virtual bool WriteData(const UINT8* data, size_t size) = 0;
    };
}
