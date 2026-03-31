#include <iostream>
#include <vector>
#include <windows.h>
#include "marker_processor.h"

int main()
{
  int arraySize;
  std::cout << "Array size: ";
  std::cin >> arraySize;

  std::vector<int> arr(arraySize, 0);

  int threadCount;
  std::cout << "Number of marker threads: ";
  std::cin >> threadCount;

  HANDLE startEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

  std::vector<ThreadContext> contexts(threadCount);
  std::vector<HANDLE> threads(threadCount);

  for (int i = 0; i < threadCount; i++)
  {
    contexts[i].data = arr.data();
    contexts[i].size = arraySize;
    contexts[i].id = i + 1;
    contexts[i].startEvent = startEvent;
    contexts[i].stuckEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    contexts[i].continueEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    contexts[i].stopEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    threads[i] = CreateThread(nullptr, 0, marker_thread, &contexts[i], 0, nullptr);
  }

  SetEvent(startEvent);

  std::vector<bool> active(threadCount, true);
  int activeCount = threadCount;

  while (activeCount > 0)
  {
    std::vector<HANDLE> stuckHandles;
    for (int i = 0; i < threadCount; i++)
      if (active[i]) stuckHandles.push_back(contexts[i].stuckEvent);

    WaitForMultipleObjects(
      static_cast<DWORD>(stuckHandles.size()),
      stuckHandles.data(),
      TRUE,
      INFINITE);

    std::cout << "\nArray state:" << std::endl;
    print_array(arr);

    int killId = -1;
    while (true)
    {
      std::cout << "Enter thread id to terminate (1.." << threadCount << "): ";
      std::cin >> killId;
      int idx = killId - 1;
      if (idx >= 0 && idx < threadCount && active[idx]) break;
      std::cout << "Thread " << killId << " is not active. Try again." << std::endl;
    }

    int killIdx = killId - 1;
    SetEvent(contexts[killIdx].stopEvent);
    WaitForSingleObject(threads[killIdx], INFINITE);
    CloseHandle(threads[killIdx]);
    threads[killIdx] = nullptr;
    active[killIdx] = false;
    activeCount--;

    std::cout << "\nArray after thread " << killId << " cleared its marks:" << std::endl;
    print_array(arr);

    for (int i = 0; i < threadCount; i++)
      if (active[i]) SetEvent(contexts[i].continueEvent);
  }

  CloseHandle(startEvent);
  for (int i = 0; i < threadCount; i++)
  {
    CloseHandle(contexts[i].stuckEvent);
    CloseHandle(contexts[i].continueEvent);
    CloseHandle(contexts[i].stopEvent);
  }

  return 0;
}
