#include "TeOggVorbisEncoder.h"
#include "Utility/TeDataStream.h"
#include "Audio/TeAudioUtility.h"
#include "Utility/TeFrameAllocator.h"

namespace te
{
// Writes to the internal cached buffer and flushes it if needed
#define WRITE_TO_BUFFER(data, length)					\
    if ((_bufferOffset + length) > BUFFER_SIZE)			\
        Flush();										\
                                                        \
    if(length > BUFFER_SIZE)							\
        _writeCallback(data, length);					\
    else												\
    {													\
        memcpy(_buffer + _bufferOffset, data, length);	\
        _bufferOffset += length;						\
    }

    OggVorbisEncoder::~OggVorbisEncoder()
    {
        Close();
    }

    bool OggVorbisEncoder::Open(std::function<void(UINT8*, UINT32)> writeCallback, UINT32 sampleRate, UINT32 bitDepth,
        UINT32 numChannels)
    {
        _numChannels = numChannels;
        _bitDepth = bitDepth;
        _writeCallback = writeCallback;
        _closed = false;

        ogg_stream_init(&_oggState, std::rand());
        vorbis_info_init(&_vorbisInfo);

        // Automatic bitrate management with quality 0.4 (~128 kbps for 44 KHz stereo sound)
        INT32 status = vorbis_encode_init_vbr(&_vorbisInfo, numChannels, sampleRate, 0.4f);
        if (status != 0)
        {
            TE_DEBUG("Failed to write Ogg Vorbis file.");
            Close();
            return false;
        }

        vorbis_analysis_init(&_vorbisState, &_vorbisInfo);
        vorbis_block_init(&_vorbisState, &_vorbisBlock);

        // Generate header
        vorbis_comment comment;
        vorbis_comment_init(&comment);

        ogg_packet headerPacket, commentPacket, codePacket;
        status = vorbis_analysis_headerout(&_vorbisState, &comment, &headerPacket, &commentPacket, &codePacket);
        vorbis_comment_clear(&comment);

        if (status != 0)
        {
            TE_DEBUG("Failed to write Ogg Vorbis file.");
            Close();
            return false;
        }

        // Write header
        ogg_stream_packetin(&_oggState, &headerPacket);
        ogg_stream_packetin(&_oggState, &commentPacket);
        ogg_stream_packetin(&_oggState, &codePacket);

        ogg_page page;
        while (ogg_stream_flush(&_oggState, &page) > 0)
        {
            WRITE_TO_BUFFER(page.header, page.header_len);
            WRITE_TO_BUFFER(page.body, page.body_len);
        }

        return true;
    }

    void OggVorbisEncoder::Write(UINT8* samples, UINT32 numSamples)
    {
        static const UINT32 WRITE_LENGTH = 1024;

        UINT32 numFrames = numSamples / _numChannels;
        while (numFrames > 0)
        {
            UINT32 numFramesToWrite = std::min(numFrames, WRITE_LENGTH);
            float** buffer = vorbis_analysis_buffer(&_vorbisState, numFramesToWrite);

            if (_bitDepth == 8)
            {
                for (UINT32 i = 0; i < numFramesToWrite; i++)
                {
                    for (UINT32 j = 0; j < _numChannels; j++)
                    {
                        INT8 sample = *(INT8*)samples;
                        float encodedSample = sample / 127.0f;
                        buffer[j][i] = encodedSample;

                        samples++;
                    }
                }
            }
            else if (_bitDepth == 16)
            {
                for (UINT32 i = 0; i < numFramesToWrite; i++)
                {
                    for (UINT32 j = 0; j < _numChannels; j++)
                    {
                        INT16 sample = *(INT16*)samples;
                        float encodedSample = sample / 32767.0f;
                        buffer[j][i] = encodedSample;

                        samples += 2;
                    }
                }
            }
            else if (_bitDepth == 24)
            {
                for (UINT32 i = 0; i < numFramesToWrite; i++)
                {
                    for (UINT32 j = 0; j < _numChannels; j++)
                    {
                        INT32 sample = AudioUtility::Convert24To32Bits(samples);
                        float encodedSample = sample / 2147483647.0f;
                        buffer[j][i] = encodedSample;

                        samples += 3;
                    }
                }
            }
            else if (_bitDepth == 32)
            {
                for (UINT32 i = 0; i < numFramesToWrite; i++)
                {
                    for (UINT32 j = 0; j < _numChannels; j++)
                    {
                        INT32 sample = *(INT32*)samples;
                        float encodedSample = sample / 2147483647.0f;
                        buffer[j][i] = encodedSample;

                        samples += 4;
                    }
                }
            }
            else
                assert(false);

            // Signal how many frames were written
            vorbis_analysis_wrote(&_vorbisState, numFramesToWrite);
            WriteBlocks();

            numFrames -= numFramesToWrite;
        }
    }

    void OggVorbisEncoder::WriteBlocks()
    {
        while (vorbis_analysis_blockout(&_vorbisState, &_vorbisBlock) == 1)
        {
            // Analyze and determine optimal bitrate
            vorbis_analysis(&_vorbisBlock, nullptr);
            vorbis_bitrate_addblock(&_vorbisBlock);

            // Write block into ogg packets
            ogg_packet packet;
            while (vorbis_bitrate_flushpacket(&_vorbisState, &packet))
            {
                ogg_stream_packetin(&_oggState, &packet);

                // If new page, write it to the internal buffer
                ogg_page page;
                while (ogg_stream_flush(&_oggState, &page) > 0)
                {
                    WRITE_TO_BUFFER(page.header, page.header_len);
                    WRITE_TO_BUFFER(page.body, page.body_len);
                }
            }
        }
    }

    void OggVorbisEncoder::Flush()
    {
        if (_bufferOffset > 0 && _writeCallback != nullptr)
            _writeCallback(_buffer, _bufferOffset);

        _bufferOffset = 0;
    }

    void OggVorbisEncoder::Close()
    {
        if (_closed)
            return;

        // Mark end of data and flush any remaining data in the buffers
        vorbis_analysis_wrote(&_vorbisState, 0);
        WriteBlocks();
        Flush();

        ogg_stream_clear(&_oggState);
        vorbis_block_clear(&_vorbisBlock);
        vorbis_dsp_clear(&_vorbisState);
        vorbis_info_clear(&_vorbisInfo);

        _closed = true;
    }

    SPtr<MemoryDataStream> OggVorbisEncoder::PCMToOggVorbis(UINT8* samples, const AudioDataInfo& info, UINT32& size)
    {
        struct EncodedBlock
        {
            UINT8* data;
            UINT32 size;
        };

        Vector<EncodedBlock> blocks;
        UINT32 totalEncodedSize = 0;
        auto writeCallback = [&](UINT8* buffer, UINT32 size)
        {
            EncodedBlock newBlock;
            newBlock.data = te_frame_allocate(size);
            newBlock.size = size;

            memcpy(newBlock.data, buffer, size);
            blocks.push_back(newBlock);
            totalEncodedSize += size;
        };

        te_frame_mark();

        OggVorbisEncoder writer;
        writer.Open(writeCallback, info.SampleRate, info.BitDepth, info.NumChannels);

        writer.Write(samples, info.NumSamples);
        writer.Close();

        auto output = te_shared_ptr_new<MemoryDataStream>(totalEncodedSize);
        UINT32 offset = 0;
        for (auto& block : blocks)
        {
            memcpy(output->Data() + offset, block.data, block.size);
            offset += block.size;

            te_frame_free(block.data);
        }

        te_frame_clear();

        size = totalEncodedSize;
        return output;
    }
}
