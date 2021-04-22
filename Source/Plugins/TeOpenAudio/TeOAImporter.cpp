#include "TeOAImporter.h"
#include "Utility/TeDataStream.h"
#include "Utility/TeFileSystem.h"
#include "Audio/TeAudioClipImportOptions.h"
#include "Audio/TeAudioUtility.h"
#include "TeOggVorbisDecoder.h"
#include "TeOggVorbisEncoder.h"
#include "TeWaveDecoder.h"
#include "TeFLACDecoder.h"

#include <filesystem>

namespace te
{
    OAImporter::OAImporter()
        : BaseImporter()
    {
        _extensions.push_back(u8"wav");
        _extensions.push_back(u8"ogg");
        _extensions.push_back(u8"flac");
    }

    bool OAImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), tolower);
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> OAImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<AudioClipImportOptions>();
    }

    SPtr<Resource> OAImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        AudioDataInfo info;
        size_t size = 0;
        UINT32 bytesPerSample = 0;
        UINT32 bufferSize = 0;
        SPtr<MemoryDataStream> sampleStream;
        {
            Lock lock = FileScheduler::GetLock(filePath);
            SPtr<FileStream> file = te_shared_ptr_new<FileStream>(filePath);

            if (file->Fail())
            {
                TE_DEBUG("Cannot open file: " + filePath);
                return nullptr;
            }

            size = file->Size();
            if (size > std::numeric_limits<UINT32>::max())
            {
                TE_DEBUG("File size larger than supported!");
                return nullptr;
            }

            String extension = file->GetExtension();
            ToLowerCase(extension);

            UPtr<AudioDecoder> reader;
            if (extension == u8".ogg")
                reader = te_unique_ptr_new<OggVorbisDecoder>();
            else if (extension == u8".wav")
                reader = te_unique_ptr_new<WaveDecoder>();
            else if (extension == u8".flac")
                reader = te_unique_ptr_new<FLACDecoder>();

            if (reader == nullptr)
                return nullptr;

            if (!reader->IsValid(file))
                return nullptr;

            if (!reader->Open(file, info))
                return nullptr;
            
            bytesPerSample = info.BitDepth / 8;
            bufferSize = info.NumSamples * bytesPerSample;

            sampleStream = te_shared_ptr_new<MemoryDataStream>(bufferSize);
            reader->Read(sampleStream->Data(), info.NumSamples);
        }

        SPtr<const AudioClipImportOptions> clipIO = std::static_pointer_cast<const AudioClipImportOptions>(importOptions);

        // If 3D, convert to mono
        if (clipIO->Is3D && info.NumChannels > 1)
        {
            UINT32 numSamplesPerChannel = info.NumSamples / info.NumChannels;

            UINT32 monoBufferSize = numSamplesPerChannel * bytesPerSample;
            auto monoStream = te_shared_ptr_new<MemoryDataStream>(monoBufferSize);

            AudioUtility::ConvertToMono(sampleStream->Data(), monoStream->Data(), info.BitDepth, numSamplesPerChannel, info.NumChannels); 

            info.NumSamples = numSamplesPerChannel;
            info.NumChannels = 1;

            sampleStream = monoStream;
            bufferSize = monoBufferSize;
        }

        // Convert bit depth if needed
        if (clipIO->BitDepth != info.BitDepth)
        {
            UINT32 outBufferSize = info.NumSamples * (clipIO->BitDepth / 8);
            auto outStream = te_shared_ptr_new<MemoryDataStream>(outBufferSize);

            AudioUtility::ConvertBitDepth(sampleStream->Data(), info.BitDepth, outStream->Data(), clipIO->BitDepth, info.NumSamples);

            info.BitDepth = clipIO->BitDepth;

            sampleStream = outStream;
            bufferSize = outBufferSize;
        }

        // Encode to Ogg Vorbis if needed
        if (clipIO->Format == AudioFormat::VORBIS)
        {
            // Note: If the original source was in Ogg Vorbis we could just copy it here, but instead we decode to PCM and
            // then re-encode which is redundant. If later we decide to copy be aware that the engine encodes Ogg in a
            // specific quality, and the the import source might have lower or higher bitrate/quality.
            sampleStream = OggVorbisEncoder::PCMToOggVorbis(sampleStream->Data(), info, bufferSize);
        }

        AUDIO_CLIP_DESC clipDesc;
        clipDesc.BitDepth = info.BitDepth;
        clipDesc.Format = clipIO->Format;
        clipDesc.Frequency = info.SampleRate;
        clipDesc.NumChannels = info.NumChannels;
        clipDesc.ReadMode = clipIO->ReadMode;
        clipDesc.Is3D = clipIO->Is3D;

        SPtr<AudioClip> clip = AudioClip::_createPtr(sampleStream, bufferSize, info.NumSamples, clipDesc);
        clip->SetName(std::filesystem::path(filePath).filename().generic_string());
        clip->SetPath(filePath);

        return clip;
    }
}
