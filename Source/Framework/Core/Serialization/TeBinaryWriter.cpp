#include "Serialization/TeBinaryWriter.h"
#include "Serialization/TeSerializable.h"

#include <cstring>
#include <stdexcept>

namespace te
{
    BinaryWriter::BinaryWriter()
    {
        _memoryStream = te_new<MemoryDataStream>();
        _stream = _memoryStream;
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
        size_t bytesWritten = _stream->Write(data, static_cast<uint32_t>(size));
        _writtenSize = std::max(_writtenSize, static_cast<size_t>(_stream->Tell()));
        return bytesWritten != 0;
    }

    Vector<UINT8> BinaryWriter::GetWrittenBuffer() const
    {
        if (_memoryStream == nullptr)
            throw std::logic_error("Cannot retrieve a buffer from a file-backed BinaryWriter");

        Vector<UINT8> buffer(_writtenSize);
        if (_writtenSize > 0)
            std::memcpy(buffer.data(), _memoryStream->Data(), _writtenSize);

        return buffer;
    }

    void BinaryWriter::WriteHeader()
    {
        WriteRaw<uint32_t>(TE_VERSION_MAJOR);
        WriteRaw<uint32_t>(TE_VERSION_MINOR);
    }
}
