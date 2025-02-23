#include "Serialization/TeStreamWriter.h"

namespace te
{
    StreamWriter::~StreamWriter()
    { }

    void StreamWriter::WriteBuffer(const UINT8* buffer, size_t size)
    {
        WriteData(reinterpret_cast<const UINT8*>(&size), sizeof(size_t));
        WriteData(reinterpret_cast<const UINT8*>(buffer), size);
    }

    void StreamWriter::WriteZero(uint64_t size)
    {
        const UINT8 zero = 0;
        for (uint64_t i = 0; i < size; i++)
            WriteData(&zero, 1);
    }

    void StreamWriter::WriteString(const std::string& string)
    {
        size_t size = string.size();
        WriteData(reinterpret_cast<const UINT8*>(&size), sizeof(size_t));
        WriteData(reinterpret_cast<const UINT8*>(string.data()), sizeof(char) * size);
    }
}
