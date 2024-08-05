#include "Serialization/TeBinarySerializer.h"

#include "Serialization/TeSerializable.h"

namespace te
{
    BinarySerializer::BinarySerializer()
    {
        _stream = te_new<MemoryDataStream>();
    }
    
    BinarySerializer::BinarySerializer(std::filesystem::path& path)
        : Serializer()
        , _path(path)
    {
        _stream = te_new<FileStream>(path.generic_string(), DataStream::AccessMode::WRITE);
    }

    BinarySerializer::~BinarySerializer()
    {
        _stream->Close();
        te_delete(_stream);
    }

    bool BinarySerializer::WriteData(const UINT8* data, size_t size)
	{
		_stream->Write(data, static_cast<UINT32>(size));
		return true;
	}
}
