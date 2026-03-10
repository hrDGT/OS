#include "array_processor.h"
#include <iostream>

MinMaxResult compute_min_max(const std::vector<int>& values)
{
  MinMaxResult result{};
  if (values.empty()) return result;
  int minValue = values[0];
  int maxValue = values[0];
  for (int i = 1; i < values.size(); i++)
  {
    if (values[i] < minValue) minValue = values[i];
    if (values[i] > maxValue) maxValue = values[i];
  }
  result.minValue = minValue;
  result.maxValue = maxValue;
  return result;
}

double compute_average(const std::vector<int>& values)
{
  if (values.empty()) return 0.0;
  long long sum = 0;
  for (int v : values) sum += v;
  return static_cast<double>(sum) / values.size();
}

DWORD WINAPI min_max_thread(LPVOID param)
{
  SharedData* shared = static_cast<SharedData*>(param);
  int minValue = shared->data[0];
  int maxValue = shared->data[0];
  for (int i = 1; i < shared->size; ++i)
  {
    if (shared->data[i] < minValue) minValue = shared->data[i];
    if (shared->data[i] > maxValue) maxValue = shared->data[i];
    Sleep(7);
  }
  shared->minValue = minValue;
  shared->maxValue = maxValue;
  std::cout << "Min: " << minValue << std::endl;
  std::cout << "Max: " << maxValue << std::endl;
  return 0;
}

DWORD WINAPI average_thread(LPVOID param)
{
  SharedData* shared = static_cast<SharedData*>(param);
  long long sum = 0;
  for (int i = 0; i < shared->size; ++i)
  {
    sum += shared->data[i];
    Sleep(12);
  }
  shared->average = static_cast<double>(sum) / shared->size;
  std::cout << "Average: " << shared->average << std::endl;
  return 0;
}
