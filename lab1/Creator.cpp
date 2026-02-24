#include <iostream>
#include <fstream>
#include "employee.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Wrong argument number" << std::endl;
    return 1;
  }

  const char* file = argv[1];
  int count = std::atoi(argv[2]);

  std::ofstream of(file, std::ios::binary);

  employee emp;
  
  for (int i = 0; i < count; i++) {
    std::cout << "Enter data for new employee" << std::endl;
    std::cin >> emp.num >> emp.name >> emp.hours;

    of.write(reinterpret_cast<char*>(&emp), sizeof(emp));
  }

  of.close();

  return 0;
}
