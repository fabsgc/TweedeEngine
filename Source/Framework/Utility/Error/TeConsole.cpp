#include "Error/TeConsole.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include <io.h>
#endif

namespace te
{
    TE_MODULE_STATIC_MEMBER(Console)

    Console::Console(bool createConsole)
        : _createConsole(createConsole)
    {}

    void Console::OnStartUp()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED && TE_PLATFORM == TE_PLATFORM_WIN32
        if (!_createConsole)
            return;

        _savedStdin = _dup(_fileno(stdin));
        _savedStdout = _dup(_fileno(stdout));
        _savedStderr = _dup(_fileno(stderr));
        if (_savedStdin == -1 || _savedStdout == -1 || _savedStderr == -1)
        {
            if (_savedStdin != -1)
                _close(_savedStdin);
            if (_savedStdout != -1)
                _close(_savedStdout);
            if (_savedStderr != -1)
                _close(_savedStderr);
            _savedStdin = _savedStdout = _savedStderr = -1;
            TE_ASSERT_ERROR(false, "Failed to preserve standard streams before creating the console.");
            return;
        }

        _streamsRedirected = true;
        AllocConsole();

        FILE* consoleStdin = nullptr;
        FILE* consoleStdout = nullptr;
        FILE* consoleStderr = nullptr;
        freopen_s(&consoleStdin, "CONIN$", "r", stdin);
        freopen_s(&consoleStdout, "CONOUT$", "w", stdout);
        freopen_s(&consoleStderr, "CONOUT$", "w", stderr);
#endif
    }

    void Console::OnShutDown()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED && TE_PLATFORM == TE_PLATFORM_WIN32
        if (!_createConsole || !_streamsRedirected)
            return;

        fflush(stdout);
        fflush(stderr);
        const bool stdinRestored = _dup2(_savedStdin, _fileno(stdin)) != -1;
        const bool stdoutRestored = _dup2(_savedStdout, _fileno(stdout)) != -1;
        const bool stderrRestored = _dup2(_savedStderr, _fileno(stderr)) != -1;
        _close(_savedStdin);
        _close(_savedStdout);
        _close(_savedStderr);
        _savedStdin = _savedStdout = _savedStderr = -1;
        _streamsRedirected = false;
        clearerr(stdin);
        clearerr(stdout);
        clearerr(stderr);
        FreeConsole();

        TE_ASSERT_ERROR(stdinRestored && stdoutRestored && stderrRestored,
            "Failed to restore standard streams after destroying the console.");
#endif
    }

    Console& gConsole()
    {
        return static_cast<Console&>(Console::Instance());
    }
}
