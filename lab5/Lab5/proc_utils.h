#include <windows.h>
#include <vector>
#include <string>

std::vector<PROCESS_INFORMATION> launchClients(int count, const std::string& clientExeName);
void waitForClientsAndClose(std::vector<PROCESS_INFORMATION>& procs);
