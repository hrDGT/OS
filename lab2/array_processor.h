#pragma once
#include <vector>
#include <windows.h>

struct SharedData
{
  int* data;
  int size;
  int minValue;
  int maxValue;
  double average;
};

struct MinMaxResult
{
  int minValue;
  int maxValue;
};

MinMaxResult compute_min_max(const std::vector<int>& values);
double compute_average(const std::vector<int>& values);

DWORD WINAPI min_max_thread(LPVOID param);
DWORD WINAPI average_thread(LPVOID param);

