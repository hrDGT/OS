#include "lock_utils.h"
#include <iostream>

bool lockRangeShared(HANDLE hFile, const LARGE_INTEGER& offset) {
    OVERLAPPED ov = {};
    ov.Offset = offset.LowPart;
    ov.OffsetHigh = offset.HighPart;

    if (!LockFileEx(hFile, 0, 0, (DWORD)sizeof(employee), 0, &ov)) {
        std::cerr << "LockFileEx(shared) failed: " << GetLastError() << "\n";

        return false;
    }

    return true;
}

bool lockRangeExclusive(HANDLE hFile, const LARGE_INTEGER& offset) {
    OVERLAPPED ov = {};
    ov.Offset = offset.LowPart;
    ov.OffsetHigh = offset.HighPart;

    if (!LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, (DWORD)sizeof(employee), 0, &ov)) {
        std::cerr << "LockFileEx(exclusive) failed: " << GetLastError() << "\n";

        return false;
    }

    return true;
}

bool unlockRange(HANDLE hFile, const LARGE_INTEGER& offset) {
    OVERLAPPED ov = {};
    ov.Offset = offset.LowPart;
    ov.OffsetHigh = offset.HighPart;

    if (!UnlockFileEx(hFile, 0, (DWORD)sizeof(employee), 0, &ov)) {
        std::cerr << "UnlockFileEx failed: " << GetLastError() << "\n";

        return false;
    }

    return true;
}
