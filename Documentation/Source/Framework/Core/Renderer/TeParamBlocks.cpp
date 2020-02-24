#include "Renderer/TeParamBlocks.h"
#include "RenderAPI/TeGpuParam.h"

namespace te
{
    ParamBlock::~ParamBlock()
    {
        ParamBlockManager::UnregisterBlock(this);
    }

    Vector<ParamBlock*> ParamBlockManager::sToInitialize;

    ParamBlockManager::ParamBlockManager()
    {
        for (auto& entry : sToInitialize)
            entry->Initialize();

        sToInitialize.clear();
    }

    void ParamBlockManager::RegisterBlock(ParamBlock* paramBlock)
    {
        if (IsStarted())
            paramBlock->Initialize();
        else
            sToInitialize.push_back(paramBlock);
    }

    void ParamBlockManager::UnregisterBlock(ParamBlock* paramBlock)
    {
        auto iterFind = std::find(sToInitialize.begin(), sToInitialize.end(), paramBlock);
        if (iterFind != sToInitialize.end())
            sToInitialize.erase(iterFind);
    }
}
