#pragma once

#include "TeOAPrerequisites.h"
#include "TeAudioDecoder.h"
#include "vorbis/vorbisfile.h"

namespace te
{
	/** Information used by the active decoder. */
	struct OggDecoderData
	{
		OggDecoderData() = default;

		SPtr<DataStream> Stream;
		UINT32 Offset = 0;
	};

	/** Used for reading Ogg Vorbis audio data. */
	class OggVorbisDecoder : public AudioDecoder
	{
	public:
		OggVorbisDecoder();
		virtual ~OggVorbisDecoder();

		/** @copydoc AudioDecoder::open */
		bool Open(const SPtr<DataStream>& stream, AudioDataInfo& info, UINT32 offset = 0) override;

		/** @copydoc AudioDecoder::read */
		UINT32 Read(UINT8* samples, UINT32 numSamples) override;

		/** @copydoc AudioDecoder::seek */
		void Seek(UINT32 offset) override;

		/** @copydoc AudioDecoder::isValid */
		bool IsValid(const SPtr<DataStream>& stream, UINT32 offset = 0) override;
	private:
		OggDecoderData _decoderData;
		OggVorbis_File _oggVorbisFile;
		UINT32 _channelCount = 0;
	};
}
