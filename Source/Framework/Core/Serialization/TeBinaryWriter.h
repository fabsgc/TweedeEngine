#pragma once

#include "TeCorePrerequisites.h"

#include "Serialization/TeStreamWriter.h"
#include "Utility/TeDataStream.h"

#include <iostream>
#include <filesystem>

// TODO Serialization : handle Endianness

namespace te
{
    class TE_CORE_EXPORT BinaryWriter : public StreamWriter
    {
    public:
        BinaryWriter();
        
        BinaryWriter(const std::filesystem::path& path);
        
        ~BinaryWriter();

        /**
         * Returns the bytes written by an in-memory writer.
         *
         * @throws std::logic_error If this writer writes to a file.
         */
        Vector<UINT8> GetWrittenBuffer() const;

        bool IsStreamGood() const final { return _stream->Good(); }
        
        uint64_t GetStreamPosition() const override final { return _stream->Tell(); }
        
        void SetStreamPosition(uint64_t position) final { _stream->Seek(position); }

    private:
        bool WriteData(const uint8_t* data, size_t size) final;

        void WriteHeader();

    private:
        std::filesystem::path _path;
        DataStream* _stream;
        MemoryDataStream* _memoryStream = nullptr;
        size_t _writtenSize = 0;
    };
}
