#include <iostream>
#include <fstream>
#include "employee.h"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Wrong argument number" << std::endl;
    return 1;
  }

  const char* infile = argv[1];
  const char* outfile = argv[2];
  int rate = std::atoi(argv[3]);

  std::ifstream ifs(infile);
  std::ofstream ofs(outfile);

  employee emp;

  ofs << "Отчёт по файлу " << infile << std::endl;
  ofs << "Номер сотрудника, имя сотрудника, часы, зарплата" << std::endl;

  while (ifs.read(reinterpret_cast<char*>(&emp), sizeof(emp)))
    ofs << emp.num << " " << emp.name << " " << emp.hours << " " << emp.hours * rate << std::endl;

  ifs.close();
  ofs.close();

  return 0;
}