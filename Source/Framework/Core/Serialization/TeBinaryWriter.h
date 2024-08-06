#pragma once

#include "TeCorePrerequisites.h"

#include "Serialization/TeStreamWriter.h"
#include "Utility/TeDataStream.h"

#include <iostream>
#include <filesystem>

namespace te
{
    class TE_CORE_EXPORT BinaryWriter : public StreamWriter
    {
    public:
        BinaryWriter();
        
        BinaryWriter(std::filesystem::path& path);
        
        ~BinaryWriter();

        bool IsStreamGood() const final { return _stream->Good(); }
        
        UINT64 GetStreamPosition() const override final { return _stream->Tell(); }
        
        void SetStreamPosition(UINT64 position) final { _stream->Seek(position); }

    private:
        bool WriteData(const UINT8* data, size_t size) final;

        void WriteHeader();

    private:
        std::filesystem::path _path;
        DataStream* _stream;
    };
}
