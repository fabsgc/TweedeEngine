#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include <istream>

namespace te
{
    /** Supported encoding types for strings. */
    enum class StringEncoding
    {
        UTF8 = 1,
        UTF16 = 2
    };

    /**
     * General purpose class used for encapsulating the reading and writing of data from and to various sources using a
     * common interface.
     */
    class TE_UTILITY_EXPORT DataStream
    {
    public:
        enum AccessMode
        {
            READ = 1,
            WRITE = 2
        };

        /** Creates an unnamed stream. */
        DataStream(UINT16 accessMode = READ)
            : _access(accessMode)
        { }

        /** Creates a named stream. */
        DataStream(const String& name, UINT16 accessMode = READ)
            : _name(name)
            , _access(accessMode) 
        { }

        virtual ~DataStream() = default;

        const String& GetName() const { return _name; }
        UINT16 GetAccessMode() const { return _access; }

        virtual bool IsReadable() const { return (_access & READ) != 0; }
        virtual bool IsWriteable() const { return (_access & WRITE) != 0; }

        /** Checks whether the stream reads/writes from a file system. */
        virtual bool IsFile() const = 0;

        /**
         * Read the requisite number of bytes from the stream, stopping at the end of the file. Advances
         * the read pointer.
         *
         * @param[in]	buf		Pre-allocated buffer to read the data into.
         * @param[in]	count	Number of bytes to read.
         * @return				Number of bytes actually read.
         *
         * @note	Stream must be created with READ access mode.
         */
        virtual size_t Read(void* buf, size_t count) const = 0;

        /**
         * Write the requisite number of bytes to the stream and advance the write pointer.
         *
         * @param[in]	buf		Buffer containing bytes to write.
         * @param[in]	count	Number of bytes to write.
         * @return				Number of bytes actually written.
         *
         * @note	Stream must be created with WRITE access mode.
         */
        virtual size_t Write(const void* buf, size_t count) = 0;

        /**
         * Returns a string containing the entire stream.
         *
         * @return	String data encoded as UTF-8.
         *
         * @note	This is a convenience method for text streams only, allowing you to retrieve a String object containing
         *			all the data in the stream.
         */
        virtual String GetAsString();

        /**
         * Returns a wide string containing the entire stream.
         *
         * @return	Wide string encoded as specified by current platform.
         *
         * @note	This is a convenience method for text streams only, allowing you to retrieve a WString object
         *			containing all the data in the stream.
         */
        virtual WString GetAsWString();

        /**
         * Reads bits from the stream into the provided buffer from the current cursor location and advances the cursor.
         * If the stream doesn't support per-bit reads, data size will be rounded up to nearest byte.
         *
         * @param[out]	data	Buffer to read the data from. Must have enough capacity to store @p count bits.
         * @param[in]	count	Number of bits to read.
         * @return				Number of bits actually read.
         *
         * @note	Stream must be created with READ access mode.
         */
        virtual size_t ReadBits(uint8_t* data, uint32_t count);

        /**
         * Writes bits from the provided buffer into the stream at the current cursor location and advances the cursor.
         * If the stream doesn't support per-bit writes, data size will be rounded up to nearest byte.
         *
         * @param[in]	data	Buffer to write the data from. Must have enough capacity to store @p count bits.
         * @param[in]	count	Number of bits to write.
         * @return				Number of bits actually written.
         *
         * @note	Stream must be created with WRITE access mode.
         */
        virtual size_t WriteBits(const uint8_t* data, uint32_t count);

        /**
         * Writes the provided narrow string to the steam. String is convered to the required encoding before being written.
         *
         * @param[in]	string		String containing narrow characters to write, encoded as UTF8.
         * @param[in]	encoding	Encoding to convert the string to before writing.
         */
        virtual void WriteString(const String& string, StringEncoding encoding = StringEncoding::UTF8);

        /**
         * Writes the provided wide string to the steam. String is convered to the required encoding before being written.
         *
         * @param[in]	string		String containing wide characters to write, encoded as specified by platform for
         * 							wide characters.
         * @param[in]	encoding	Encoding to convert the string to before writing.
         */
        virtual void WriteString(const WString& string, StringEncoding encoding = StringEncoding::UTF8);

        /**
         * Skip a defined number of bytes. This can also be a negative value, in which case the file pointer rewinds a
         * defined number of bytes.
         */
        virtual void Skip(size_t count) = 0;

        /** Repositions the read point to a specified byte. */
        virtual void Seek(size_t pos) = 0;

        /** Returns the current byte offset from beginning. */
        virtual size_t Tell() const = 0;

        /**
         * Aligns the read/write cursor to a byte boundary. @p count determines the alignment in bytes. Note the
         * requested alignment might not be achieved if count > 1 and it would move the cursor past the capacity of the
         * buffer, as the cursor will be clamped to buffer end regardless of alignment.
         */
        virtual void Align(uint32_t count = 1);

        /** Returns true if the stream has reached the end. */
        virtual bool Eof() const = 0;

        /** Returns the total size of the data to be read from the stream, or 0 if this is indeterminate for this stream. */
        size_t Size() const { return _size; }

        /** Close the stream. This makes further operations invalid. */
        virtual void Close() = 0;

    protected:
        static const UINT32 StreamTempSize;

        String _name;
        size_t _size = 0;
        UINT16 _access;

    };

    class TE_UTILITY_EXPORT FileStream : public DataStream
    {
    public:
        FileStream(UINT16 mode = READ);
        FileStream(const String& path, UINT16 mode = READ);
        ~FileStream();

        /** @copydoc DataStream::IsFile */
        bool IsFile() const override { return true; }

        /** @copydoc DataStream::Read */
        virtual size_t Read(void* buf, size_t count) const override;

        /** @copydoc DataStream::Write */
        virtual size_t Write(const void* buf, size_t count) override;

        /** @copydoc DataStream::Skip */
        virtual void Skip(size_t count) override;
    
        /** @copydoc DataStream::Seek */
        virtual void Seek(size_t pos);
        
        /** @copydoc DataStream::Tell */
        virtual size_t Tell() const override;

        /** @copydoc DataStream::Eof */
        virtual bool Eof() const override;

        /** @copydoc DataStream::Close */
        virtual void Close() override;

        /** Returns true if open has failed */
        bool Fail();

        /** Returns the path given in parameter. */
        const String& GetPath() const { return _path; }

        /** Returns the system dependant path computed internally. */
        String GetPlatformPath() const {return  _internalPath; }

        /** Returns extension with "." */
        String GetExtension() const { return _extension; }

    protected:
        void SetInternalPath();
        void SetExtension();
        void CalculteSize();
        void Open();

    protected:
        String _path;
        String _internalPath;
        String _extension;

        SPtr<std::istream> _inStream;
        SPtr<std::ifstream> _FStreamRO;
        SPtr<std::fstream> _FStream;
    };

    /** Data stream for handling data from memory. */
    class TE_UTILITY_EXPORT MemoryDataStream : public DataStream
    {
    public:
        /**
         * Initializes an empty memory stream. As data is written the stream will grow its internal memory storage
         * automatically.
         */
        MemoryDataStream();

        /**
         * Initializes a stream with some initial capacity. If more bytes than capacity is written, the stream will
         * grow its internal memory storage.
         *
         * @param[in]	capacity	Number of bytes to initially allocate for the internal memory storage.
         */
        MemoryDataStream(size_t capacity);

        /** @copydoc DataStream::isFile */
        virtual bool IsFile() const override { return false; }

        /** @copydoc DataStream::read */
        virtual size_t Read(void* buf, size_t count) const override;

        /** @copydoc DataStream::write */
        virtual size_t Write(const void* buf, size_t count) override;

        /** @copydoc DataStream::Skip */
        virtual void Skip(size_t count) override;

        /** @copydoc DataStream::Seek */
        virtual void Seek(size_t pos) override;

        /** @copydoc DataStream::Tell */
        virtual size_t Tell() const override;

        /** @copydoc DataStream::Eof */
        virtual bool Eof() const override;

        /** @copydoc DataStream::Close */
        virtual void Close() override;

        /** Get a pointer to the start of the memory block this stream holds. */
        uint8_t* data() const { return _data; }

        /** Get a pointer to the current position in the memory block this stream holds. */
        uint8_t* cursor() const { return _cursor; }

        

    protected:
        /** Reallocates the internal buffer making enough room for @p numBytes. */
        void Realloc(size_t numBytes);

        uint8_t* _data = nullptr;
        mutable uint8_t* _cursor = nullptr;
        uint8_t* _end = nullptr;

        bool _ownsMemory = true;
    };
}
