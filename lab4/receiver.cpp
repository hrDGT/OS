#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "message_queue.h"

int main()
{
  std::string filename;
  int capacity;
  std::cout << "Enter binary file name: ";
  std::cin >> filename;
  std::cout << "Enter number of records (queue capacity): ";
  std::cin >> capacity;

  if (capacity <= 0)
  {
    std::cerr << "Capacity must be positive.\n";
    return 1;
  }

  if (!create_queue_file(filename, capacity))
  {
    std::cerr << "Failed to create queue file '" << filename << "' (error " << GetLastError() << ")" << std::endl;
    return 1;
  }
  std::cout << "Queue file '" << filename << "' created (capacity: " << capacity << " messages)." << std::endl;

  HANDLE hMutex = CreateMutexA(NULL, FALSE, MUTEX_NAME);
  HANDLE hItems = CreateSemaphoreA(NULL, 0, capacity, ITEMS_SEM_NAME);
  HANDLE hSpaces = CreateSemaphoreA(NULL, capacity, capacity, SPACES_SEM_NAME);
  HANDLE hReady = CreateSemaphoreA(NULL, 0, 128, READY_SEM_NAME);

  if (!hMutex || !hItems || !hSpaces || !hReady)
  {
    std::cerr << "Failed to create synchronisation objects (error " << GetLastError() << ").\n" << "Make sure no other Receiver is already running." << std::endl;
    return 1;
  }

  int senderCount;
  std::cout << "Enter number of Sender processes: ";
  std::cin >> senderCount;

  if (senderCount <= 0)
  {
    std::cerr << "Sender count must be positive.\n";
    return 1;
  }

  char selfPath[MAX_PATH];
  GetModuleFileNameA(NULL, selfPath, MAX_PATH);
  std::string senderExe(selfPath);
  std::size_t slashPos = senderExe.rfind('\\');
  senderExe = (slashPos != std::string::npos) ? senderExe.substr(0, slashPos + 1) + "sender.exe" : "sender.exe";

  std::vector<HANDLE> hProcesses(senderCount);
  for (int i = 0; i < senderCount; i++)
  {
    std::string cmdLine = "\"" + senderExe + "\" " + filename;
    std::vector<char> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back('\0');

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    if (!CreateProcessA(
        senderExe.c_str(), cmdBuf.data(),
        NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si, &pi))
    {
      std::cerr << "Failed to launch sender #" << (i + 1) << " (error " << GetLastError() << ").\n" << "Make sure sender.exe is in the same directory." << std::endl;
      return 1;
    }
    hProcesses[i] = pi.hProcess;
    CloseHandle(pi.hThread);
    std::cout << "Launched Sender #" << (i + 1) << "  (PID " << pi.dwProcessId << ")" << std::endl;
  }

  std::cout << "Waiting for " << senderCount << " sender(s) to report ready..." << std::endl;
  for (int i = 0; i < senderCount; i++)
  {
    WaitForSingleObject(hReady, INFINITE);
    std::cout << "  Sender " << (i + 1) << "/" << senderCount << " ready." << std::endl;
  }
  std::cout << "All senders ready.\n";

  while (true)
  {
    std::cout << "\nCommand (r=read, q=quit): ";
    char cmd;
    std::cin >> cmd;

    if (cmd == 'q' || cmd == 'Q')
    {
      std::cout << "Receiver exiting.\n";
      break;
    }
    if (cmd == 'r' || cmd == 'R')
    {
      std::cout << "Waiting for message...\n";
      std::string msg;
      if (dequeue_message(filename, msg, hMutex, hItems, hSpaces))
        std::cout << "Received: \"" << msg << "\"\n";
      else
        std::cout << "Read failed.\n";
    }
  }

  for (auto& h : hProcesses) CloseHandle(h);
  CloseHandle(hMutex);
  CloseHandle(hItems);
  CloseHandle(hSpaces);
  CloseHandle(hReady);

  return 0;
}
