#include "Serialization/TeBinaryReader.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    BinaryReader::BinaryReader(void* data, size_t size)
        : _data(data)
        , _dataSize(size)
    {
        _stream = te_new<MemoryDataStream>(data, size);
    }
    
    BinaryReader::BinaryReader(const std::filesystem::path& path)
        : _path(path)
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

    bool BinaryReader::ReadData(uint8_t* dest, size_t size)
    {
        return _stream->Read(dest, size);
    }

    void BinaryReader::ReadHeader()
    {
        ReadRaw<uint32_t>(_versionMajor);
        ReadRaw<uint32_t>(_versionMinor);
    }
}
