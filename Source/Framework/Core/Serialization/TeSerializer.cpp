#include "Serialization/TeSerializer.h"

namespace te
{
    Serializer::~Serializer()
    { }

    void Serializer::WriteBuffer(UINT8* buffer, size_t size)
	{
        WriteData(reinterpret_cast<const UINT8*>(size), sizeof(size_t));
		WriteData(reinterpret_cast<const UINT8*>(buffer), size);
	}

	void Serializer::WriteZero(uint64_t size)
	{
		const UINT8 zero = 0;
		for (uint64_t i = 0; i < size; i++)
			WriteData(&zero, 1);
	}

	void Serializer::WriteString(const std::string& string)
	{
		size_t size = string.size();
		WriteData(reinterpret_cast<const UINT8*>(&size), sizeof(size_t));
		WriteData(reinterpret_cast<const UINT8*>(string.data()), sizeof(char) * string.size());
	}
}
