#pragma once

#include "TeCorePrerequisites.h"

#include "Serialization/TeSerializer.h"
#include "Utility/TeDataStream.h"

#include "Json/json.h"

#include <iostream>
#include <filesystem>

namespace te
{
    class TE_CORE_EXPORT BinarySerializer : public Serializer
    {
    public:
        BinarySerializer();
        BinarySerializer(std::filesystem::path& path);
        ~BinarySerializer();

        bool IsStreamGood() const final { return _stream->Good(); }
		UINT64 GetStreamPosition() const override final { return _stream->Tell(); }
		void SetStreamPosition(UINT64 position) final { _stream->Seek(position); }
		bool WriteData(const UINT8* data, size_t size) final;

    private:
        std::filesystem::path _path;
		DataStream* _stream;
    };
}
