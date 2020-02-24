#include "Error/TeConsole.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Console)

    Console::Console()
    {}

    void Console::OnStartUp()
    {
#if TE_DEBUG_MODE == 1 && TE_PLATFORM == TE_PLATFORM_WIN32
        AllocConsole();

        fpstdin = stdin;
        fpstdout = stdout;
        fpstderr = stderr;

        freopen_s(&fpstdin, "CONIN$", "r", stdin);
        freopen_s(&fpstdout, "CONOUT$", "w", stdout);
        freopen_s(&fpstderr, "CONOUT$", "w", stderr);
#endif
    }

    void Console::OnShutDown()
    {
#if TE_DEBUG_MODE == 1 && TE_PLATFORM == TE_PLATFORM_WIN32
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();
#endif
    }

    Console& gConsole()
    {
        return static_cast<Console&>(Console::Instance());
    }
}