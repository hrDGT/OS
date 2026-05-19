#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "../file_utils.h"
#include "../common.h"

static void removeFile(const char* name) {
    ::DeleteFileA(name);
}

TEST(FileUtilsExtra, FindNonexistent) {
    const char* fname = "nonexistent.dat";
    ::DeleteFileA(fname);

    LARGE_INTEGER off;
    EXPECT_FALSE(findRecordOffset(fname, 12345, off));
}

TEST(FileUtilsExtra, MultipleRecordsFindFirstLast) {
    const char* fname = "multi_test.dat";
    removeFile(fname);

    std::vector<employee> v;
    employee a{ 1, "", 1.0 }; strncpy_s(a.name, "A", sizeof(a.name) - 1);
    employee b{ 2, "", 2.0 }; strncpy_s(b.name, "B", sizeof(b.name) - 1);
    employee c{ 3, "", 3.0 }; strncpy_s(c.name, "C", sizeof(c.name) - 1);
    v.push_back(a); v.push_back(b); v.push_back(c);

    ASSERT_TRUE(writeEmployeesToFile(fname, v));

    LARGE_INTEGER off1, off3;
    ASSERT_TRUE(findRecordOffset(fname, 1, off1));
    ASSERT_TRUE(findRecordOffset(fname, 3, off3));
    EXPECT_NE(off1.QuadPart, off3.QuadPart);

    HANDLE h = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h, INVALID_HANDLE_VALUE);
    employee out1, out3;
    ASSERT_TRUE(readRecordAtOffset(h, off1, out1));
    ASSERT_TRUE(readRecordAtOffset(h, off3, out3));
    EXPECT_EQ(out1.num, 1);
    EXPECT_EQ(out3.num, 3);
    CloseHandle(h);

    removeFile(fname);
}

TEST(FileUtilsExtra, PartialReadFailure) {
    const char* fname = "partial.dat";
    removeFile(fname);

    HANDLE h = CreateFileA(fname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h, INVALID_HANDLE_VALUE);
    DWORD written;
    const char buf[5] = { 0,1,2,3,4 };
    ASSERT_TRUE(WriteFile(h, buf, sizeof(buf), &written, NULL));
    CloseHandle(h);

    LARGE_INTEGER off; off.QuadPart = 0;
    HANDLE h2 = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h2, INVALID_HANDLE_VALUE);
    employee out;
    EXPECT_FALSE(readRecordAtOffset(h2, off, out));
    CloseHandle(h2);

    removeFile(fname);
}
