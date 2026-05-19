#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <Windows.h>
#include "../file_utils.h"
#include "../common.h"

static void removeFile(const char* name) {
    ::DeleteFileA(name);
}

TEST(FileUtils, WriteReadModify) {
    const char* fname = "test_employees.dat";
    removeFile(fname);

    std::vector<employee> v;
    employee a{ 1, "", 10.5 };
    strncpy_s(a.name, "Alice", sizeof(a.name) - 1);
    employee b{ 2, "", 20.0 };
    strncpy_s(b.name, "Bob", sizeof(b.name) - 1);
    v.push_back(a);
    v.push_back(b);

    ASSERT_TRUE(writeEmployeesToFile(fname, v));
    ASSERT_TRUE(printFileContents(fname));

    LARGE_INTEGER off;
    ASSERT_TRUE(findRecordOffset(fname, 2, off));

    HANDLE h = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h, INVALID_HANDLE_VALUE);
    employee out;
    ASSERT_TRUE(readRecordAtOffset(h, off, out));
    EXPECT_EQ(out.num, 2);
    EXPECT_STREQ(out.name, "Bob");
    EXPECT_DOUBLE_EQ(out.hours, 20.0);
    CloseHandle(h);

    HANDLE h2 = CreateFileA(fname, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h2, INVALID_HANDLE_VALUE);
    employee ne = out;
    strncpy_s(ne.name, "Robert", sizeof(ne.name) - 1);
    ne.hours = 25.5;
    ASSERT_TRUE(writeRecordAtOffset(h2, off, ne));
    CloseHandle(h2);

    HANDLE h3 = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT_NE(h3, INVALID_HANDLE_VALUE);
    employee out2;
    ASSERT_TRUE(readRecordAtOffset(h3, off, out2));
    EXPECT_EQ(out2.num, 2);
    EXPECT_STREQ(out2.name, "Robert");
    EXPECT_DOUBLE_EQ(out2.hours, 25.5);
    CloseHandle(h3);

    removeFile(fname);
}
