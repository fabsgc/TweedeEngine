#include "TeFileStream.h"

namespace te
{
    FileStream::FileStream(const String& path, AccessMode mode)
        : _path(path)
        , _mode(mode)
    {
        SetInternalPath();
        SetExtension();
        Open();
    }

    FileStream::FileStream()
        : _path("")
        , _mode(AccessMode::READ)
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

        if ((_mode & READ) != 0)
            mode |= std::ios::in;

        if (((_mode & WRITE) != 0))
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

    size_t FileStream::Read(void* buf, size_t count)
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

    String FileStream::GetAsString()
    {
        // Read the entire buffer - ideally in one read, but if the size of
        // the buffer is unknown, do multiple fixed size reads.
        size_t bufSize = (_size > 0 ? _size : 4096);
        char * pBuf = static_cast<char*>(te_allocate(static_cast<UINT32>(bufSize)));
        // Ensure read from begin of stream
        Seek(0);
        String result;
        while (!Eof())
        {
            size_t nr = Read(pBuf, bufSize);
            result.append(pBuf, nr);
        }
        te_delete(pBuf);
        return result;
    }

    void FileStream::Skip(size_t count)
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_mode & WRITE) != 0))
            _FStream->seekp(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
        else
            _inStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
    }

    void FileStream::Seek(size_t pos)
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_mode & WRITE) != 0))
            _FStream->seekp(static_cast<std::ifstream::pos_type>(pos), std::ios::beg);
        else
            _inStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
    }

    size_t FileStream::Tell()
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_mode & WRITE) != 0))
            return (size_t)_FStream->tellp();

        return (size_t)_inStream->tellg();
    }

    bool FileStream::Eof() const
    {
        return _inStream->eof();
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
            //_inStream->ignore(std::numeric_limits<std::streamsize>::max());
            //_size = _inStream->gcount() + 1; //We add the terminal charactet \0

            //_inStream->clear(); 
            //_inStream->seekg(0, std::ios_base::beg);
        }
        else
        {
            _size = 0;
        }
    }
}
