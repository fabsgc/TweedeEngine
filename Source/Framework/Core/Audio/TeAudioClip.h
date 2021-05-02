#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    /** Valid internal engine audio formats. */
    enum class AudioFormat
    {
        PCM, /**< Pulse code modulation audio ("raw" uncompressed audio). */
        VORBIS /**< Vorbis compressed audio. */
    };

    /** Modes that determine how and when is audio data read. */
    enum class AudioReadMode
    {
        /** Entire audio clip will be loaded and decompressed. Uses most memory but has lowest CPU impact. */
        LoadDecompressed,
        /**
         * Entire audio clip will be loaded, but will be decompressed while playing. Uses medium amount of memory and has
         * the highest CPU impact.
         */
        LoadCompressed,
        /**
         * Audio will be slowly streamed from the disk, and decompressed if needed. Uses very little memory, and has either
         * low or high CPU impact depending if the audio is in a compressed format. Since data is streamed from the disk,
         * read speeds could also be a bottleneck.
         */
        Stream
    };

    /** Descriptor used for initializing an audio clip. */
    struct AUDIO_CLIP_DESC
    {
        /** Determines how is audio data read. */
        AudioReadMode ReadMode = AudioReadMode::LoadDecompressed;

        /** Determines in which format is the audio data in. */
        AudioFormat Format = AudioFormat::PCM;

        /** Sample rate (frequency) of the audio data. */
        UINT32 Frequency = 44100;

        /** Number of bits per sample. Not used for compressed formats. */
        UINT32 BitDepth = 16;
        
        /** Number of channels. Each channel has its own step of samples. */
        UINT32 NumChannels = 2;

        /** Determines should the audio clip be played using 3D positioning. Only valid for mono audio. */
        bool Is3D = true;
    };

    /**
     * Audio clip stores audio data in a compressed or uncompressed format. Clips can be provided to audio sources or
     * other audio methods to be played.
     */
    class TE_CORE_EXPORT AudioClip : public Resource
    {
    public:
        virtual ~AudioClip() = default;

        /** Returns the size of a single sample, in bits. */
        UINT32 GetBitDepth() const { return _desc.BitDepth; }
        
        /** Returns how many samples per second is the audio encoded in. */
        UINT32 GetFrequency() const { return _desc.Frequency; }

        /** Returns the number of channels provided by the clip. */
        UINT32 GetNumChannels() const { return _desc.NumChannels; }

        /**
         * Returns in which format is audio data stored in.
         *
         * @see	AudioFormat
         */
        AudioFormat GetFormat() const { return _desc.Format; }

        /**
         * Returns how is the audio data read/decoded.
         *
         * @see	AudioReadMode
         */
        AudioReadMode GetReadMode() const { return _desc.ReadMode; }

        /** Returns the length of the audio clip, in seconds. */
        float GetLength() const { return _length; }

        /** Returns the total number of samples in the clip (includes all channels). */
        UINT32 GetNumSamples() const { return _numSamples; }

        /** Determines will the clip be played a spatial 3D sound, or as a normal sound (for example music). */
        bool Is3D() const { return _desc.Is3D; }

        /**
         * Creates a new AudioClip and populates it with provided samples.
         *
         * @param[in]	samples		Data streams containing the samples to load. Data will be read starting from the current
         *							position in the stream. The samples should be in audio format as specified in the
         *							@p desc parameter. Ownership of the data stream is taken by the audio clip and the
         *							caller must not close it manually.
         * @param[in]	streamSize	Number of bytes to read from the @p samples stream.
         * @param[in]	numSamples	Total number of samples (including all channels).
         * @param[in]	desc		Descriptor containing meta-data for the provided samples.
         *
         * @note	If the provided samples are in PCM format, they should be signed integers of provided bit depth.
         */
        static HAudioClip Create(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
            const AUDIO_CLIP_DESC& desc);

    public: // ***** INTERNAL ******

        /** Creates a new AudioClip without initializing it. Use create() for normal use. */
        static SPtr<AudioClip> _createPtr(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
            const AUDIO_CLIP_DESC& desc);

        /** @} */
    protected:
        AudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc);

        /** @copydoc Resource::Initialize */
        void Initialize() override;

        /** Returns original audio data. Only available if @p keepSourceData has been provided on creation. */
        virtual SPtr<DataStream> GetSourceStream(UINT32& size) = 0;

    protected:
        AUDIO_CLIP_DESC _desc;
        UINT32 _numSamples;
        UINT32 _streamSize;
        UINT32 _streamOffset = 0;
        float _length = 0.0f;
        SPtr<DataStream> _streamData;

    public:
        /**
         * Creates an AudioClip with no samples. You must populate its data manually followed by a call to initialize().
         *
         * @note	For serialization use only.
         */
        static SPtr<AudioClip> CreateEmpty();
    };
}
