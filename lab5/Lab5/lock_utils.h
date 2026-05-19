#include "common.h"
#include <windows.h>

bool lockRangeShared(HANDLE hFile, const LARGE_INTEGER& offset);
bool lockRangeExclusive(HANDLE hFile, const LARGE_INTEGER& offset);
bool unlockRange(HANDLE hFile, const LARGE_INTEGER& offset);
