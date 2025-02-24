#pragma once

#include "TeCorePrerequisites.h"

#include "Serialization/TeStreamReader.h"
#include "Utility/TeDataStream.h"

#include <iostream>
#include <filesystem>

// TODO Serialization : handle Endianness

namespace te
{
    class TE_CORE_EXPORT BinaryReader : public StreamReader
    {
    public:
        BinaryReader(void* data, size_t size);

        BinaryReader(const std::filesystem::path& path);

        ~BinaryReader();

        bool IsStreamGood() const final { return _stream->Good(); }

        uint64_t GetStreamPosition() const override final { return _stream->Tell(); }

        void SetStreamPosition(uint64_t position) final { _stream->Seek(position); }

        uint32_t GetVersionMajor() const { return _versionMajor; }

        uint32_t GetVersionMinor() const { return _versionMinor; }

    private:
        bool ReadData(uint8_t* dest, size_t size) override;

        void ReadHeader();

    private:
        std::filesystem::path _path;
        void* _data = nullptr;
        size_t _dataSize = 0;

        DataStream* _stream = {};
        uint32_t _versionMajor = 0;
        uint32_t _versionMinor = 0;
    };
}
