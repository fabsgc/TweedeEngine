#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Utility/TeModule.h"

namespace te
{
    class TE_UTILITY_EXPORT Console : public Module<Console>
    {
    public:
        explicit Console(bool createConsole = true);
        ~Console() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(Console)

    protected:
        void OnStartUp() override;
        void OnShutDown() override;

    protected:
        bool _createConsole;
        int _savedStdin = -1;
        int _savedStdout = -1;
        int _savedStderr = -1;
        bool _streamsRedirected = false;
        bool _stdinRedirected = false;
        bool _stdoutRedirected = false;
        bool _stderrRedirected = false;
    };

    Console& gConsole();
}
