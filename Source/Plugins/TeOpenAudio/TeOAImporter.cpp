#include "TeOAImporter.h"
#include "Utility/TeDataStream.h"
#include "Utility/TeFileSystem.h"
#include "Audio/TeAudioClipImportOptions.h"
#include "TeOggVorbisDecoder.h"

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
            /*if (extension == u8".wav")
                reader = te_unique_ptr_new<WaveDecoder>();
            else if (extension == u8".flac")
                reader = te_unique_ptr_new<FLACDecoder>();*/

            if (reader == nullptr)
                return nullptr;

            if (!reader->IsValid(file))
                return nullptr;

            if (!reader->Open(file, info))
                return nullptr;

            sampleStream = te_shared_ptr_new<MemoryDataStream>(bufferSize);
            reader->Read(sampleStream->data(), info.NumSamples);
        }

        return SPtr<Resource>();
    }
}
