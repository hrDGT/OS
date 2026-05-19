#include <windows.h>

HANDLE createServerPipeInstance(const char* pipeName);
bool waitForClientConnect(HANDLE hPipe);
HANDLE connectToServerPipe(const char* pipeName, DWORD timeoutMs = 5000);
