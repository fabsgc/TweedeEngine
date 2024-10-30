#include "Serialization/TeBinaryReader.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    BinaryReader::BinaryReader()
    {
        _stream = te_new<MemoryDataStream>();
    }
    
    BinaryReader::BinaryReader(const std::filesystem::path& path)
        : StreamReader()
        , _path(path)
    {
        _stream = te_new<FileStream>(path.generic_string(), DataStream::AccessMode::READ);

        if (IsStreamGood())
        {
            ReadHeader();
        }
    }

    BinaryReader::~BinaryReader()
    {
        _stream->Close();
        te_delete(_stream);
    }

    bool BinaryReader::ReadData(UINT8* dest, size_t size)
    {
        return _stream->Read(dest, size);
    }

    void BinaryReader::ReadHeader()
    {
        ReadRaw<UINT32>(_versionMajor);
        ReadRaw<UINT32>(_versionMinor);
    }
}
