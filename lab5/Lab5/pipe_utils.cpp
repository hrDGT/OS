#include "pipe_utils.h"
#include <iostream>

HANDLE createServerPipeInstance(const char* pipeName) {
    HANDLE hPipe = CreateNamedPipeA(
        pipeName,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        4096, 4096,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe failed: " << GetLastError() << "\n";
    }

    return hPipe;
}

bool waitForClientConnect(HANDLE hPipe) {
    if (hPipe == INVALID_HANDLE_VALUE) {
        return false;
    }

    BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!connected) {
        std::cerr << "ConnectNamedPipe failed or client not connected: " << GetLastError() << "\n";

        return false;
    }

    return true;
}

HANDLE connectToServerPipe(const char* pipeName, DWORD timeoutMs) {
    DWORD start = GetTickCount();

    while (true) {
        HANDLE h = CreateFileA(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

        if (h != INVALID_HANDLE_VALUE) {
            return h;
        }

        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "CreateFileA for pipe failed: " << GetLastError() << "\n";

            return INVALID_HANDLE_VALUE;
        }

        if (!WaitNamedPipeA(pipeName, timeoutMs)) {
            std::cerr << "WaitNamedPipe timeout or failed: " << GetLastError() << "\n";

            return INVALID_HANDLE_VALUE;
        }

        if (GetTickCount() - start > timeoutMs) {
            std::cerr << "Timeout connecting to pipe\n";

            return INVALID_HANDLE_VALUE;
        }
    }
}
