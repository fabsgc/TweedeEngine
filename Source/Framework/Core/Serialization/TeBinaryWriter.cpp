#include "Serialization/TeBinaryWriter.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    BinaryWriter::BinaryWriter()
    {
        _stream = te_new<MemoryDataStream>();
    }
    
    BinaryWriter::BinaryWriter(std::filesystem::path& path)
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

    bool BinaryWriter::WriteData(const UINT8* data, size_t size)
    {
        _stream->Write(data, static_cast<UINT32>(size));
        return true;
    }

    void BinaryWriter::WriteHeader()
    {
        WriteRaw<UINT32>(TE_VERSION_MAJOR);
        WriteRaw<UINT32>(TE_VERSION_MINOR);
    }
}
