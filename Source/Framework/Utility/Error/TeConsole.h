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

        TE_MODULE_STATIC_HEADER_MEMBER(Console)

    protected:
        void OnStartUp() override;
        void OnShutDown() override;

    protected:
        FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;
    };

    Console& gConsole();
}