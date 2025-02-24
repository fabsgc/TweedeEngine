#include "Serialization/TeBinaryWriter.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    BinaryWriter::BinaryWriter()
    {
        _stream = te_new<MemoryDataStream>();
    }
    
    BinaryWriter::BinaryWriter(const std::filesystem::path& path)
        : StreamWriter()
        , _path(path)
    {
        _stream = te_new<FileStream>(path.generic_string(), DataStream::AccessMode::WRITE);

        if (IsStreamGood())
        {
            WriteHeader();
        }
    }

    BinaryWriter::~BinaryWriter()
    {
        _stream->Close();
        te_delete(_stream);
    }

    bool BinaryWriter::WriteData(const uint8_t* data, size_t size)
    {
        return _stream->Write(data, static_cast<uint32_t>(size));
    }

    void BinaryWriter::WriteHeader()
    {
        WriteRaw<uint32_t>(TE_VERSION_MAJOR);
        WriteRaw<uint32_t>(TE_VERSION_MINOR);
    }
}
