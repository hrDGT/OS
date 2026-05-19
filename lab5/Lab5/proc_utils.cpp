#include "proc_utils.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <io.h>

std::vector<PROCESS_INFORMATION> launchClients(int count, const std::string& clientExePath) {
    std::vector<PROCESS_INFORMATION> procs;
    procs.reserve(count);

    if (_access(clientExePath.c_str(), 0) != 0) {
        char cwd[260];
        GetCurrentDirectoryA(sizeof(cwd), cwd);
        std::cerr << "Client executable not found: " << clientExePath << "\n";
        std::cerr << "Current dir: " << cwd << "\n";

        return procs;
    }

    for (int i = 0; i < count; ++i) {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::ostringstream cmd;
        cmd << "\"" << clientExePath << "\" " << (i + 1);
        std::string cmdStr = cmd.str();
        char* cmdLine = _strdup(cmdStr.c_str());

        if (!cmdLine) {
            std::cerr << "Memory allocation failed for command line\n";

            continue;
        }

        BOOL ok = CreateProcessA(
            clientExePath.c_str(),
            cmdLine,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        );

        free(cmdLine);

        if (!ok) {
            std::cerr << "CreateProcess failed for client " << i + 1 << ", Error: " << GetLastError() << "\n";

            continue;
        }

        procs.push_back(pi);
        std::cout << "Launched client " << i + 1 << " (PID=" << pi.dwProcessId << ")\n";
    }

    return procs;
}

void waitForClientsAndClose(std::vector<PROCESS_INFORMATION>& procs) {
    if (procs.empty()) {
        return;
    }

    std::vector<HANDLE> handles;
    handles.reserve(procs.size());

    for (auto& pi : procs) {
        handles.push_back(pi.hProcess);
    }

    const DWORD MAX_WAIT = MAXIMUM_WAIT_OBJECTS;
    size_t idx = 0;

    while (idx < handles.size()) {
        size_t batch = std::min<size_t>(MAX_WAIT, handles.size() - idx);
        DWORD res = WaitForMultipleObjects((DWORD)batch, &handles[idx], TRUE, INFINITE);

        if (res == WAIT_FAILED) {
            std::cerr << "WaitForMultipleObjects failed: " << GetLastError() << "\n";

            break;
        }

        idx += batch;
    }

    for (auto& pi : procs) {
        if (pi.hProcess) {
            CloseHandle(pi.hProcess);
        }

        if (pi.hThread) {
            CloseHandle(pi.hThread);
        }
    }

    procs.clear();
}
