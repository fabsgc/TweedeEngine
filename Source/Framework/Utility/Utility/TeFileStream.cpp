#include "TeFileStream.h"

namespace te
{
    FileStream::FileStream(String path, AccessMode mode, PathType pathType)
        : _path(path)
        , _mode(mode)
        , _pathType(pathType)
    {
        SetInternalPath();
        Open();
    }

    FileStream::~FileStream()
    {
        Close();
    }

    void FileStream::Open()
    {
        std::ios::openmode mode = std::ios::binary;

        if ((_mode & READ) != 0)
        {
            mode |= std::ios::in;
        }

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
            TE_DEBUG("Cannot open file: " + _path, __FILE__, __LINE__);
            return;
        }

        _inStream->seekg(0, std::ios_base::end);
        _size = (size_t)_inStream->tellg();
        _inStream->seekg(0, std::ios_base::beg);
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
        }

        return written;
    }

    void FileStream::Skip(size_t count)
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_mode & WRITE) != 0))
        {
            _FStream->seekp(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
        }
        else
        {
            _inStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
        }
    }

    void FileStream::Seek(size_t pos)
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_mode & WRITE) != 0))
        {
            _FStream->seekp(static_cast<std::ifstream::pos_type>(pos), std::ios::beg);
        }
        else
        {
            _inStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
        }
    }

    size_t FileStream::Tell()
    {
        _inStream->clear(); // Clear fail status in case eof was set

        if (((_mode & WRITE) != 0))
        {
            return (size_t)_FStream->tellp();
        }

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
            {
                _FStreamRO->close();
            }

            if (_FStream)
            {
                _FStream->flush();
                _FStream->close();
            }
        }
    }


    String FileStream::GetPlatformPath()
    {
        return _internalPath;
    }

    void FileStream::SetInternalPath()
    {
        _internalPath = _path;

        switch (_pathType)
        {
        case PathType::Windows:
            ParseWindowsPath();
            break;
        case PathType::Unix:
            ParseUnixPath();
            break;
        default:
#if TE_PLATFORM == TE_PLATFORM_WIN32
            ParseWindowsPath();
#elif TE_PLATFORM == TE_PLATFORM == TE_PLATFORM_LINUX
            ParseUnixPath();
#else
            static_assert(false, "Unsupported platform for path.");
#endif
            break;
        }
    }

    void FileStream::ParseWindowsPath()
    {

    }

    void FileStream::ParseUnixPath()
    {

    }

    String FileStream::BuildWindowsPath()
    {
        return _internalPath;
    }

    String FileStream::BuildUnixPath()
    {
        return _internalPath;
    }
}