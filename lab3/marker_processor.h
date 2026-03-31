#pragma once
#include <vector>
#include <windows.h>

struct ThreadContext
{
  int* data;
  int size;
  int id;
  HANDLE startEvent;
  HANDLE stuckEvent;
  HANDLE continueEvent;
  HANDLE stopEvent;
};

bool try_mark(int* data, int idx, int id);
int count_marked(const int* data, int size, int id);
void clear_marks(int* data, int size, int id);

void print_array(const std::vector<int>& arr);

DWORD WINAPI marker_thread(LPVOID param);
