#include "Error/TeConsole.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include <io.h>
#endif

namespace te
{
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED && TE_PLATFORM == TE_PLATFORM_WIN32
    namespace
    {
        int PreserveStream(FILE* stream, bool& failed)
        {
            const int descriptor = _fileno(stream);
            if (descriptor < 0)
                return -1;

            const int saved = _dup(descriptor);
            if (saved == -1)
                failed = true;
            return saved;
        }

        bool RestoreStream(FILE* stream, int saved, bool redirected)
        {
            bool restored = true;
            if (redirected)
            {
                if (saved != -1)
                    restored = _dup2(saved, _fileno(stream)) != -1;
                else
                    restored = fclose(stream) == 0;
            }
            else if (saved != -1)
                restored = false;

            if (saved != -1 && _close(saved) == -1)
                restored = false;
            if (redirected && saved != -1 && restored)
                clearerr(stream);
            return restored;
        }
    }
#endif

    TE_MODULE_STATIC_MEMBER(Console)

    Console::Console(bool createConsole)
        : _createConsole(createConsole)
    {}

    void Console::OnStartUp()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED && TE_PLATFORM == TE_PLATFORM_WIN32
        if (!_createConsole)
            return;

        bool preserveFailed = false;
        _savedStdin = PreserveStream(stdin, preserveFailed);
        _savedStdout = PreserveStream(stdout, preserveFailed);
        _savedStderr = PreserveStream(stderr, preserveFailed);

        if (!AllocConsole())
        {
            if (_savedStdin != -1)
                _close(_savedStdin);
            if (_savedStdout != -1)
                _close(_savedStdout);
            if (_savedStderr != -1)
                _close(_savedStderr);
            _savedStdin = _savedStdout = _savedStderr = -1;
            return;
        }

        _streamsRedirected = true;

        FILE* consoleStdin = nullptr;
        FILE* consoleStdout = nullptr;
        FILE* consoleStderr = nullptr;
        _stdinRedirected = freopen_s(&consoleStdin, "CONIN$", "r", stdin) == 0;
        _stdoutRedirected = freopen_s(&consoleStdout, "CONOUT$", "w", stdout) == 0;
        _stderrRedirected = freopen_s(&consoleStderr, "CONOUT$", "w", stderr) == 0;

        std::cin.clear();
        std::cout.clear();
        std::cerr.clear();
        std::clog.clear();
        std::wcin.clear();
        std::wcout.clear();
        std::wcerr.clear();
        std::wclog.clear();

        if (preserveFailed)
            TE_DEBUG("Failed to preserve one or more standard streams before creating the console.");
#endif
    }

    void Console::OnShutDown()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED && TE_PLATFORM == TE_PLATFORM_WIN32
        if (!_createConsole || !_streamsRedirected)
            return;

        if (_stdoutRedirected)
            fflush(stdout);
        if (_stderrRedirected)
            fflush(stderr);
        const bool stdinRestored = RestoreStream(stdin, _savedStdin, _stdinRedirected);
        const bool stdoutRestored = RestoreStream(stdout, _savedStdout, _stdoutRedirected);
        const bool stderrRestored = RestoreStream(stderr, _savedStderr, _stderrRedirected);
        _savedStdin = _savedStdout = _savedStderr = -1;
        _streamsRedirected = false;
        _stdinRedirected = _stdoutRedirected = _stderrRedirected = false;
        FreeConsole();

        if (!stdinRestored || !stdoutRestored || !stderrRestored)
            TE_DEBUG("Failed to restore standard streams after destroying the console.");
#endif
    }

    Console& gConsole()
    {
        return static_cast<Console&>(Console::Instance());
    }
}
