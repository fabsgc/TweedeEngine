#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Utility/TeModule.h"

namespace te
{
    class TE_UTILITY_EXPORT Console : public Module<Console>
    {
    public:
        Console();
        ~Console() = default;

    protected:
        void OnStartUp() override;
        void OnShutDown() override;

    protected:
        int _hCrtIn;
        int _hCrtOut;
    };

    Console& gConsole();
}