#pragma once

#include "TeCorePrerequisites.h"

#include "Serialization/TeStreamReader.h"
#include "Utility/TeDataStream.h"

#include <iostream>
#include <filesystem>

namespace te
{
    class TE_CORE_EXPORT BinaryReader : public StreamReader
    {
    public:
        BinaryReader();
        BinaryReader(std::filesystem::path& path);
        ~BinaryReader();

        bool IsStreamGood() const final { return _stream->Good(); }
        UINT64 GetStreamPosition() const override final { return _stream->Tell(); }
        void SetStreamPosition(UINT64 position) final { _stream->Seek(position); }

    private:
        bool ReadData(UINT8* dest, size_t size) override;

    private:
        std::filesystem::path _path;
        DataStream* _stream;
    };
}
