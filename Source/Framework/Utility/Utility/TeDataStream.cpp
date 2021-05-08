#include "TeDataStream.h"
#include "String/TeUnicode.h"
#include "Math/TeMath.h"

namespace te
{
    const UINT32 DataStream::StreamTempSize = 128;

    /** Checks does the provided buffer has an UTF32 byte order mark in little endian order. */
    bool IsUTF32LE(const UINT8* buffer)
    {
        return buffer[0] == 0xFF && buffer[1] == 0xFE && buffer[2] == 0x00 && buffer[3] == 0x00;
    }

    /** Checks does the provided buffer has an UTF32 byte order mark in big endian order. */
    bool IsUTF32BE(const UINT8* buffer)
    {
        return buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0xFE && buffer[3] == 0xFF;
    }

    /** Checks does the provided buffer has an UTF16 byte order mark in little endian order. */
    bool IsUTF16LE(const UINT8* buffer)
    {
        return buffer[0] == 0xFF && buffer[1] == 0xFE;
    }

    /**	Checks does the provided buffer has an UTF16 byte order mark in big endian order. */
    bool IsUTF16BE(const UINT8* buffer)
    {
        return buffer[0] == 0xFE && buffer[1] == 0xFF;
    }

    /**	Checks does the provided buffer has an UTF8 byte order mark. */
    bool IsUTF8(const UINT8* buffer)
    {
        return (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF);
    }

    void DataStream::Align(uint32_t count)
    {
        if (count <= 1)
            return;

        UINT32 alignOffset = (count - (Tell() & (count - 1))) & (count - 1);
        Skip(alignOffset);
    }

    String DataStream::GetAsString()
    {
        // Ensure read from begin of stream
        Seek(0);

        // Try reading header
        UINT8 headerBytes[4];
        size_t numHeaderBytes = Read(headerBytes, 4);

        size_t dataOffset = 0;
        if (numHeaderBytes >= 4)
        {
            if (IsUTF32LE(headerBytes))
                dataOffset = 4;
            else if (IsUTF32BE(headerBytes))
            {
                TE_DEBUG("UTF-32 big endian decoding not supported");
                return u8"";
            }
        }

        if (dataOffset == 0 && numHeaderBytes >= 3)
        {
            if (IsUTF8(headerBytes))
                dataOffset = 3;
        }

        if (dataOffset == 0 && numHeaderBytes >= 2)
        {
            if (IsUTF16LE(headerBytes))
                dataOffset = 2;
            else if (IsUTF16BE(headerBytes))
            {
                TE_DEBUG("UTF-16 big endian decoding not supported");
                return u8"";
            }
        }

        Seek(dataOffset);

        // Read the entire buffer - ideally in one read, but if the size of the buffer is unknown, do multiple fixed size
        // reads.
        size_t bufSize = (_size > 0 ? _size : 4096);
        char* pBuf = static_cast<char*>(te_allocate(static_cast<UINT32>(bufSize)));

        std::stringstream result;
        while (!Eof())
        {
            size_t numReadBytes = Read(pBuf, bufSize);
            result.write(pBuf, numReadBytes);
        }
        te_delete(pBuf);
        std::string string = result.str();

        switch (dataOffset)
        {
            default:
            case 0: // No BOM = assumed UTF-8
            case 3: // UTF-8
                return String(string.data(), string.length());
            case 2: // UTF-16
            {
                UINT32 numElems = (UINT32)string.length() / 2;
                return UTF8::FromUTF16(U16String((char16_t*)string.data(), numElems));
            }
            case 4: // UTF-32
            {
                UINT32 numElems = (UINT32)string.length() / 4;
                return UTF8::FromUTF32(U32String((char32_t*)string.data(), numElems));
            }
        }

        // Note: Never assuming ANSI as there is no ideal way to check for it. If required I need to
        // try reading the data and if all UTF encodings fail, assume it's ANSI. For now it should be
        // fine as most files are UTF-8 encoded.
    }

    WString DataStream::GetAsWString()
    {
        String u8string = GetAsString();
        return UTF8::ToWide(u8string);
    }

    size_t DataStream::ReadBits(uint8_t* data, uint32_t count)
    {
        uint32_t numBytes = Math::DivideAndRoundUp(count, 8U);
        return Read(data, numBytes) * 8;
    }

    size_t DataStream::WriteBits(const uint8_t* data, uint32_t count)
    {
        uint32_t numBytes = Math::DivideAndRoundUp(count, 8U);
        return Write(data, numBytes) * 8;
    }

    void DataStream::WriteString(const String& string, StringEncoding encoding)
    {
        if (encoding == StringEncoding::UTF16)
        {
            // Write BOM
            UINT8 bom[2] = { 0xFF, 0xFE };
            Write(bom, sizeof(bom));

            U16String u16string = UTF8::ToUTF16(string);
            Write(u16string.data(), u16string.length() * sizeof(char16_t));
        }
        else
        {
            // Write BOM
            UINT8 bom[3] = { 0xEF, 0xBB, 0xBF };
            Write(bom, sizeof(bom));

            Write(string.data(), string.length());
        }
    }

    void DataStream::WriteString(const WString& string, StringEncoding encoding)
    {
        if (encoding == StringEncoding::UTF16)
        {
            // Write BOM
            UINT8 bom[2] = { 0xFF, 0xFE };
            Write(bom, sizeof(bom));

            String u8string = UTF8::FromWide(string);
            U16String u16string = UTF8::ToUTF16(u8string);
            Write(u16string.data(), u16string.length() * sizeof(char16_t));
        }
        else
        {
            // Write BOM
            UINT8 bom[3] = { 0xEF, 0xBB, 0xBF };
            Write(bom, sizeof(bom));

            String u8string = UTF8::FromWide(string);
            Write(u8string.data(), u8string.length());
        }
    }

    FileStream::FileStream(const String& path, UINT16 accessMode)
        : DataStream(path, accessMode)
        , _path(path)
    {
        SetInternalPath();
        SetExtension();
        Open();
    }

    FileStream::FileStream(UINT16 accessMode)
        : DataStream(accessMode)
        , _path("")
    { }

    FileStream::~FileStream()
    {
        Close();
    }

    bool FileStream::Fail()
    {
        return _inStream->fail();
    }

    void FileStream::Open()
    {
        std::ios::openmode mode = std::ios::binary;

        if ((_access & READ) != 0)
            mode |= std::ios::in;

        if (((_access & WRITE) != 0))
        {
            mode |= std::ios::out;
            _FStream = te_shared_ptr_new<std::fstream>();
            _FStream->open(GetPlatformPath().c_str(), mode);
            _inStream = _FStream;
        }
        else
        {
            _FStreamRO = te_shared_ptr_new<std::ifstream>();
            _FStreamRO->open(GetPlatformPath().c_str(), mode);
            _inStream = _FStreamRO;
        }

        // Should check ensure open succeeded, in case fail for some reason.
        if (_inStream->fail())
        {
            TE_DEBUG("Cannot open file: " + _path);
            return;
        }

        CalculteSize();
    }

    size_t FileStream::Read(void* buf, size_t count) const
    {
        _inStream->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));
        return (size_t)_inStream->gcount();
    }

    size_t FileStream::Write(const void* buf, size_t count)
    {
        size_t written = 0;
        if (IsWriteable() && _FStream)
        {
            _FStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
            written = count;
            CalculteSize();
        }

        return written;
    }

    void FileStream::Skip(size_t count)
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_access & WRITE) != 0))
            _FStream->seekp(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
        else
            _inStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
    }

    void FileStream::Seek(size_t pos)
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_access & WRITE) != 0))
            _FStream->seekp(static_cast<std::ifstream::pos_type>(pos), std::ios::beg);
        else
            _inStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
    }

    size_t FileStream::Tell() const
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_access & WRITE) != 0))
            return (size_t)_FStream->tellp();

        return (size_t)_inStream->tellg();
    }

    bool FileStream::Eof() const
    {
        return _inStream->eof();
    }

    SPtr<DataStream> FileStream::Clone(bool copyData) const
    {
        return te_shared_ptr_new<FileStream>(_path, (AccessMode)GetAccessMode());
    }

    void FileStream::Close()
    {
        if (_inStream)
        {
            if (_FStreamRO)
                _FStreamRO->close();

            if (_FStream)
            {
                _FStream->flush();
                _FStream->close();
            }
        }
    }

    void FileStream::SetInternalPath()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        _internalPath = ReplaceAll(_path, "/", "\\");
#elif TE_PLATFORM == TE_PLATFORM_LINUX
        _internalPath = ReplaceAll(_path, "\\", "/");
#endif
    }

    void FileStream::SetExtension()
    {
        String::size_type pos = _internalPath.rfind('.');
        if (pos != String::npos)
            _extension = _internalPath.substr(pos);
        else
            _extension = String();
    }

    void FileStream::CalculteSize()
    {
        if (!_inStream->fail())
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32 // Windows
            WIN32_FILE_ATTRIBUTE_DATA attrData;
            if (GetFileAttributesExW(ToWString(_internalPath).c_str(), GetFileExInfoStandard, &attrData) == FALSE)
                TE_DEBUG("Can't get file size : " + _internalPath);

            LARGE_INTEGER li;
            li.LowPart = attrData.nFileSizeLow;
            li.HighPart = attrData.nFileSizeHigh;
            _size = (size_t)li.QuadPart;
#else
            struct stat st_buf;

            if (stat(_internalPath.c_str(), &st_buf) == 0)
            {
                _size = (size_t)st_buf.st_size;
            }
            else
            {
                TE_DEBUG("Can't get file size : " + _internalPath);
                _size = 0;
            }
#endif
            //_inStream->ignore(std::numeric_limits<std::strea_size>::max());
            //_size = _inStream->gcount() + 1; //We add the terminal charactet \0

            //_inStream->clear(); 
            //_inStream->seekg(0, std::ios_base::beg);
        }
        else
        {
            _size = 0;
        }
    }

    MemoryDataStream::MemoryDataStream()
        : DataStream(READ | WRITE)
    { }

    MemoryDataStream::MemoryDataStream(size_t capacity)
        : DataStream(READ | WRITE)
    {
        Realloc(capacity);
        _cursor = _data;
        _end = _cursor + capacity;
    }

    MemoryDataStream::MemoryDataStream(void* memory, size_t size)
		: DataStream(READ | WRITE), _ownsMemory(false)
	{
		_data = _cursor = static_cast<uint8_t*>(memory);
		_size = size;
		_end = _data + _size;
	}

	MemoryDataStream::MemoryDataStream(const MemoryDataStream& sourceStream)
		: DataStream(READ | WRITE)
	{
		// Copy data from incoming stream
		_size = sourceStream.Size();

		_data = _cursor = static_cast<uint8_t*>(te_allocate((uint32_t)_size));
		_end = _data + sourceStream.Read(_data, _size);

		assert(_end >= _cursor);
	}

	MemoryDataStream::MemoryDataStream(const SPtr<DataStream>& sourceStream)
		: DataStream(READ | WRITE)
	{
		// Copy data from incoming stream
		_size = sourceStream->Size();

		_data = _cursor = static_cast<uint8_t*>(te_allocate((uint32_t)_size));
		_end = _data + sourceStream->Read(_data, _size);

		assert(_end >= _cursor);
	}

	MemoryDataStream::MemoryDataStream(MemoryDataStream&& other) noexcept
	{
		*this = std::move(other);
	}

	MemoryDataStream::~MemoryDataStream()
	{
		Close();
	}

    MemoryDataStream& MemoryDataStream::operator= (const MemoryDataStream& other)
	{
		if (this == &other)
			return *this;

		this->_name = other._name;
		this->_access = other._access;
		
		if (!other._ownsMemory)
		{
			this->_size = other._size;
			this->_data = other._data;
			this->_cursor = other._cursor;
			this->_end = other._end;
			this->_ownsMemory = false;
		}
		else
		{
			if (_data && _ownsMemory)
				te_free(_data);

			_size = 0;
			_data = nullptr;
			_cursor = nullptr;
			_end = nullptr;

			this->_ownsMemory = true;

			Realloc(other._size);
			_end = _data + _size;
			_cursor = _data + (other._cursor - other._data);

			if (_size > 0)
				memcpy(_data, other._data, _size);
		}

		return *this;
	}

	MemoryDataStream& MemoryDataStream::operator= (MemoryDataStream&& other)
	{
		if (this == &other)
			return *this;

		if (_data && _ownsMemory)
			te_free(_data);

		this->_name = std::move(other._name);
		this->_access = std::exchange(other._access, (UINT16)0);
		this->_cursor = std::exchange(other._cursor, nullptr);
		this->_end = std::exchange(other._end, nullptr);
		this->_data = std::exchange(other._data, nullptr);
		this->_size = std::exchange(other._size, 0);
		this->_ownsMemory = std::exchange(other._ownsMemory, false);

		return *this;
	}

    size_t MemoryDataStream::Read(void* buf, size_t count) const
    {
        size_t cnt = count;

        if (_cursor + cnt > _end)
            cnt = _end - _cursor;

        if (cnt == 0)
            return 0;

        assert(cnt <= count);

        memcpy(buf, _cursor, cnt);
        _cursor += cnt;

        return cnt;
    }

    size_t MemoryDataStream::Write(const void* buf, size_t count)
    {
        size_t written = 0;
        if (IsWriteable())
        {
            written = count;

            size_t numUsedBytes = (_cursor - _data);
            size_t newSize = numUsedBytes + count;
            if (newSize > _size)
            {
                if (_ownsMemory)
                    Realloc(newSize);
                else
                    written = _size - numUsedBytes;
            }

            if (written == 0)
                return 0;

            memcpy(_cursor, buf, written);
            _cursor += written;

            _end = std::max(_cursor, _end);
        }

        return written;
    }

    void MemoryDataStream::Skip(size_t count)
    {
        assert((_cursor + count) <= _end);
        _cursor = std::min(_cursor + count, _end);
    }

    void MemoryDataStream::Seek(size_t pos)
    {
        assert((_data + pos) <= _end);
        _cursor = std::min(_data + pos, _end);
    }

    size_t MemoryDataStream::Tell() const
    {
        return _cursor - _data;
    }

    bool MemoryDataStream::Eof() const
    {
        return _cursor >= _end;
    }

    SPtr<DataStream> MemoryDataStream::Clone(bool copyData) const
    {
        if (!copyData)
            return te_shared_ptr_new<MemoryDataStream>(_data, _size);

        return te_shared_ptr_new<MemoryDataStream>(*this);
    }

    void MemoryDataStream::Close()
    {
        if (_data != nullptr)
        {
            if (_ownsMemory)
                te_free(_data);

            _data = nullptr;
        }
    }

    void MemoryDataStream::Realloc(size_t numBytes)
    {
        if (numBytes != _size)
        {
            assert(numBytes > _size);

            auto buffer = te_allocateN<uint8_t>(numBytes);
            if (_data)
            {
                _cursor = buffer + (_cursor - _data);
                _end = buffer + (_end - _data);

                memcpy(buffer, _data, _size);
                te_free(_data);
            }
            else
            {
                _cursor = buffer;
                _end = buffer;
            }

            _data = buffer;
            _size = numBytes;
        }
    }
}
