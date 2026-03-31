#include "marker_processor.h"
#include <iostream>

bool try_mark(int* data, int idx, int id)
{
  if (data[idx] != 0) return false;
  data[idx] = id;
  return true;
}

int count_marked(const int* data, int size, int id)
{
  int count = 0;
  for (int i = 0; i < size; i++)
    if (data[i] == id) count++;
  return count;
}

void clear_marks(int* data, int size, int id)
{
  for (int i = 0; i < size; i++)
    if (data[i] == id) data[i] = 0;
}

void print_array(const std::vector<int>& arr)
{
  for (int i = 0; i < static_cast<int>(arr.size()); i++)
    std::cout << "[" << i << "]=" << arr[i] << " ";
  std::cout << std::endl;
}

DWORD WINAPI marker_thread(LPVOID param)
{
  ThreadContext* ctx = static_cast<ThreadContext*>(param);

  WaitForSingleObject(ctx->startEvent, INFINITE);

  srand(ctx->id);

  while (true)
  {
    int idx = rand() % ctx->size;

    if (try_mark(ctx->data, idx, ctx->id))
    {
      Sleep(5);
      Sleep(5);
    }
    else
    {
      std::cout << "Thread " << ctx->id
                << ": marked=" << count_marked(ctx->data, ctx->size, ctx->id)
                << ", blocked at index=" << idx << std::endl;

      SetEvent(ctx->stuckEvent);

      HANDLE signals[2] = { ctx->continueEvent, ctx->stopEvent };
      DWORD result = WaitForMultipleObjects(2, signals, FALSE, INFINITE);

      if (result == WAIT_OBJECT_0 + 1)
      {
        clear_marks(ctx->data, ctx->size, ctx->id);
        return 0;
      }
    }
  }
}
