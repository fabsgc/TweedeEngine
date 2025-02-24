#include "Serialization/TeStreamReader.h"

namespace te
{
    StreamReader::~StreamReader()
    { }

    void StreamReader::ReadBuffer(UINT8** dest, size_t size)
	{
		if (size == 0)
			ReadData(reinterpret_cast<UINT8*>(&size), sizeof(size_t));

        *dest = static_cast<UINT8*>(te_allocate(static_cast<uint32_t>(size)));
		ReadData(*dest, size);
	}

	void StreamReader::ReadString(std::string& string)
	{
		size_t size = 0;
		ReadData(reinterpret_cast<UINT8*>(&size), sizeof(size_t));

		string.resize(size);
		ReadData(reinterpret_cast<UINT8*>(string.data()), sizeof(UINT8) * size);
	}
}
