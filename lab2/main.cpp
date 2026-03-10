#include <iostream>
#include <vector>
#include <windows.h>
#include "array_processor.h"

int main()
{
  int n;
  std::cin >> n;
  std::vector<int> values(n);

  for (int i = 0; i < n; i)
    std::cin >> values[i];

  SharedData shared{};
  shared.data = values.data();
  shared.size = n;

  HANDLE hMinMax = CreateThread(NULL, 0, min_max_thread, &shared, 0, NULL);
  HANDLE hAverage = CreateThread(NULL, 0, average_thread, &shared, 0, NULL);

  WaitForSingleObject(hMinMax, INFINITE);
  WaitForSingleObject(hAverage, INFINITE);

  CloseHandle(hMinMax);
  CloseHandle(hAverage);

  for (int i = 0; i < n; i++)
  {
    if (values[i] == shared.minValue || values[i] == shared.maxValue)
      values[i] = static_cast<int>(shared.average);
  }

  for (int i = 0; i < n; ++i) std::cout << values[i] << " ";
  std::cout << std::endl;

  return 0;
}
