#include "String/TeStringID.h"

namespace te
{
    const StringID StringID::NONE;

    volatile StringID::InitStatics StringID::_initStatics = StringID::InitStatics();

    StringID::InternalData* StringID::_stringHashTable[HASH_TABLE_SIZE];
    StringID::InternalData* StringID::_chunks[MAX_CHUNK_COUNT];

    UINT32 StringID::_nextId = 0;
    UINT32 StringID::_numChunks = 0;
    Mutex StringID::_sync;

    StringID::InitStatics::InitStatics()
    {
        Lock lock(_sync);

        memset(_stringHashTable, 0, sizeof(_stringHashTable));
        memset(_chunks, 0, sizeof(_chunks));

        _chunks[0] = (InternalData*)te_allocate(sizeof(InternalData) * ELEMENTS_PER_CHUNK);
        memset(_chunks[0], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);

        _numChunks++;
    }

    template<class T>
    void StringID::Construct(T const& name)
    {
        assert(StringIDUtil<T>::size(name) <= STRING_SIZE);

        UINT32 hash = CalcHash(name) & (sizeof(_stringHashTable) / sizeof(_stringHashTable[0]) - 1);
        InternalData* existingEntry = _stringHashTable[hash];
        
        while (existingEntry != nullptr)
        {
            if (StringIDUtil<T>::compare(name, existingEntry->chars))
            {
                _data = existingEntry;
                return;
            }

            existingEntry = existingEntry->next;
        }

        Lock lock(_sync);

        // Search for the value again in case other thread just added it
        existingEntry = _stringHashTable[hash];
        InternalData* lastEntry = nullptr;
        while (existingEntry != nullptr)
        {
            if (StringIDUtil<T>::compare(name, existingEntry->chars))
            {
                _data = existingEntry;
                return;
            }

            lastEntry = existingEntry;
            existingEntry = existingEntry->next;
        }

        _data = allocEntry();
        StringIDUtil<T>::copy(name, _data->chars);

        if (lastEntry == nullptr)
            _stringHashTable[hash] = _data;
        else
            lastEntry->next = _data;
    }

    template<class T>
    UINT32 StringID::CalcHash(T const& input)
    {
        UINT32 size = StringIDUtil<T>::size(input);

        UINT32 hash = 0;
        for (UINT32 i = 0; i < size; i++)
            hash = hash * 101 + input[i];

        return hash;
    }

    StringID::InternalData* StringID::allocEntry()
    {
        UINT32 chunkIdx = _nextId / ELEMENTS_PER_CHUNK;

        assert(chunkIdx < MAX_CHUNK_COUNT);
        assert(chunkIdx <= _numChunks); // Can only increment sequentially

        if (chunkIdx >= _numChunks)
        {
            _chunks[chunkIdx] = (InternalData*)te_allocate(sizeof(InternalData) * ELEMENTS_PER_CHUNK);
            memset(_chunks[chunkIdx], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);

            _numChunks++;
        }

        InternalData* chunk = _chunks[chunkIdx];
        UINT32 chunkSpecificIndex = _nextId % ELEMENTS_PER_CHUNK;

        InternalData* newEntry = &chunk[chunkSpecificIndex];
        newEntry->id = _nextId++;
        newEntry->next = nullptr;

        return newEntry;
    }

    template<>
    class StringID::StringIDUtil<const char*>
    {
    public:
        static UINT32 size(const char* const& input) { return (UINT32)strlen(input); }
        static void copy(const char* const& input, char* dest) { memcpy(dest, input, strlen(input) + 1); }
        static bool compare(const char* const& a, char* b) { return strcmp(a, b) == 0; }
    };

    template<>
    class StringID::StringIDUtil <String>
    {
    public:
        static UINT32 size(String const& input) { return (UINT32)input.length(); }
        static void copy(String const& input, char* dest)
        {
            UINT32 len = (UINT32)input.length();
            input.copy(dest, len);
            dest[len] = '\0';
        }
        static bool compare(String const& a, char* b) { return a.compare(b) == 0; }
    };

    template TE_UTILITY_EXPORT void StringID::Construct(const char* const&);
    template TE_UTILITY_EXPORT void StringID::Construct(String const&);
    
    template TE_UTILITY_EXPORT UINT32 StringID::CalcHash(const char* const&);
    template TE_UTILITY_EXPORT UINT32 StringID::CalcHash(String const&);
}
