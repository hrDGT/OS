#include <gtest/gtest.h>
#include "../lock_utils.h"
#include "../file_utils.h"
#include "../common.h"
#include <vector>

TEST(LockUtils, LockUnlockRange) {
    const char* fname = "lock_test.dat";
    ::DeleteFileA(fname);

    std::vector<employee> v;
    employee e{ 42, "", 8.0 };
    strncpy_s(e.name, "Locky", sizeof(e.name) - 1);
    v.push_back(e);
    ASSERT_TRUE(writeEmployeesToFile(fname, v));

    LARGE_INTEGER off;
    ASSERT_TRUE(findRecordOffset(fname, 42, off));

    HANDLE h = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h, INVALID_HANDLE_VALUE);

    ASSERT_TRUE(lockRangeShared(h, off));
    ASSERT_TRUE(unlockRange(h, off));

    ASSERT_TRUE(lockRangeExclusive(h, off));
    ASSERT_TRUE(unlockRange(h, off));

    CloseHandle(h);
    ::DeleteFileA(fname);
}
