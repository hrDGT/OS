#include <gtest/gtest.h>
#include <thread>
#include "../file_utils.h"
#include "../lock_utils.h"
#include "../common.h"
#include <vector>

TEST(IntegrationSimulated, ReadThenModifySequence) {
    const char* fname = "integration_sim.dat";
    ::DeleteFileA(fname);

    std::vector<employee> v;
    employee e{ 7, "", 7.0 }; strncpy_s(e.name, "Init", sizeof(e.name) - 1);
    v.push_back(e);
    ASSERT_TRUE(writeEmployeesToFile(fname, v));

    LARGE_INTEGER off;
    ASSERT_TRUE(findRecordOffset(fname, 7, off));

    HANDLE h = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h, INVALID_HANDLE_VALUE);

    // Читатель
    ASSERT_TRUE(lockRangeShared(h, off));
    employee out;
    ASSERT_TRUE(readRecordAtOffset(h, off, out));
    EXPECT_EQ(out.num, 7);
    ASSERT_TRUE(unlockRange(h, off));

    // Модификатор
    ASSERT_TRUE(lockRangeExclusive(h, off));
    employee ne = out;
    strncpy_s(ne.name, "Changed", sizeof(ne.name) - 1);
    ne.hours = 99.9;
    ASSERT_TRUE(writeRecordAtOffset(h, off, ne));
    ASSERT_TRUE(unlockRange(h, off));

    // Проверка
    HANDLE h2 = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h2, INVALID_HANDLE_VALUE);
    employee out2;
    ASSERT_TRUE(readRecordAtOffset(h2, off, out2));
    EXPECT_STREQ(out2.name, "Changed");
    EXPECT_DOUBLE_EQ(out2.hours, 99.9);
    CloseHandle(h2);

    CloseHandle(h);
    ::DeleteFileA(fname);
}
