#include "common.h"
#include <windows.h>
#include <vector>

bool writeEmployeesToFile(const char* filename, const std::vector<employee>& v);
bool printFileContents(const char* filename);
bool findRecordOffset(const char* filename, int id, LARGE_INTEGER& offset);
bool readRecordAtOffset(HANDLE hFile, const LARGE_INTEGER& offset, employee& out);
bool writeRecordAtOffset(HANDLE hFile, const LARGE_INTEGER& offset, const employee& in);
