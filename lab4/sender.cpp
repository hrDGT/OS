#include <iostream>
#include <string>
#include <windows.h>
#include "message_queue.h"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: sender <queue_filename>" << std::endl;
    return 1;
  }

  const std::string filename = argv[1];

  HANDLE hMutex = NULL;
  HANDLE hItems = NULL;
  HANDLE hSpaces = NULL;
  HANDLE hReady = NULL;

  for (int attempt = 0; attempt < 20; attempt++)
  {
    if (!hMutex) hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
    if (!hItems) hItems = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, ITEMS_SEM_NAME);
    if (!hSpaces) hSpaces = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SPACES_SEM_NAME);
    if (!hReady) hReady = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, READY_SEM_NAME);

    if (hMutex && hItems && hSpaces && hReady) break;
    Sleep(100);
  }

  if (!hMutex || !hItems || !hSpaces || !hReady)
  {
    std::cerr << "Sender: failed to open synchronisation objects from Receiver.\n" << "Make sure the Receiver is running." << std::endl;
    return 1;
  }

  ReleaseSemaphore(hReady, 1, NULL);
  std::cout << "Sender ready. Queue file: " << filename << std::endl;

  while (true)
  {
    std::cout << "\nCommand (s=send, q=quit): ";
    char cmd;
    std::cin >> cmd;

    if (cmd == 'q' || cmd == 'Q')
    {
      std::cout << "Sender exiting.\n";
      break;
    }
    if (cmd == 's' || cmd == 'S')
    {
      std::string msg;
      std::cout << "Enter message (max " << (MAX_MESSAGE_LEN - 1) << " chars): ";
      std::cin >> msg;

      std::cout << "Sending (will wait if queue is full)...\n";
      if (enqueue_message(filename, msg, hMutex, hSpaces, hItems))
        std::cout << "Message sent.\n";
      else
        std::cout << "Send failed.\n";
    }
  }

  CloseHandle(hMutex);
  CloseHandle(hItems);
  CloseHandle(hSpaces);
  CloseHandle(hReady);

  return 0;
}
