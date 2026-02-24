#include <iostream>
#include <fstream>
#include <windows.h>
#include "employee.h"

void StartProcess(const std::string& cl) {
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  if (!CreateProcessA(NULL, const_cast<char*>(cl.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    std::cerr << "Error: " << GetLastError() << "" << std::endl;

  WaitForSingleObject(pi.hProcess, INFINITE);

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
}


int main() {
  std::string binfile;
  int count;

  std::cout << "Enter binary filename: ";
  std::cin >> binfile;
  std::cout << "Enter number of records: ";
  std::cin >> count;

  StartProcess("Creator.exe " + binfile + " " + std::to_string(count));

  std::ifstream binifs(binfile, std::ios::binary);
  employee emp;

  while(binifs.read(reinterpret_cast<char*>(&emp), sizeof(emp)))
    std::cout << emp.num << " " << emp.name << " " << emp.hours << std::endl;

  binifs.close();

  std::string reportfile;
  int rate;

  std::cout << "Enter report filename: ";
  std::cin >> reportfile;
  std::cout << "Enter rate per hour: ";
  std::cin >> rate;

  StartProcess("Reporter.exe " + binfile + " " + reportfile + " " + std::to_string(rate));

  std::ifstream repportifs(reportfile);
  std::string line;
  while(getline(repportifs, line))
    std::cout << line << std::endl;

  repportifs.close();

  return 0;
}