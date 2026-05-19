#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <vector>
#include "../lock_utils.h"
#include "../file_utils.h"
#include "../common.h"

TEST(LockUtilsConcurrency, SharedAllowsMultipleReadersExclusiveBlocks) {
    const char* fname = "lock_conc.dat";
    ::DeleteFileA(fname);

    std::vector<employee> v;
    employee e{ 100, "", 5.0 }; strncpy_s(e.name, "T", sizeof(e.name) - 1);
    v.push_back(e);
    ASSERT_TRUE(writeEmployeesToFile(fname, v));

    LARGE_INTEGER off;
    ASSERT_TRUE(findRecordOffset(fname, 100, off));

    HANDLE h = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h, INVALID_HANDLE_VALUE);

    std::atomic<int> readers{ 0 };
    std::atomic<bool> writer_acquired{ false };

    auto reader = [&]() {
        ASSERT_TRUE(lockRangeShared(h, off));
        readers.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ASSERT_TRUE(unlockRange(h, off));
        };

    auto writer = [&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ASSERT_TRUE(lockRangeExclusive(h, off));
        writer_acquired = true;
        ASSERT_TRUE(unlockRange(h, off));
        };

    std::thread r1(reader), r2(reader);
    std::thread w(writer);

    r1.join(); r2.join(); w.join();

    EXPECT_EQ(readers.load(), 2);
    EXPECT_TRUE(writer_acquired.load());

    CloseHandle(h);
    ::DeleteFileA(fname);
}
