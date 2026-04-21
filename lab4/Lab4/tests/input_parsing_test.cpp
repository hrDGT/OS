#include "../utils/input_parsing.h"

#include <gtest/gtest.h>
#include <string>

using namespace std;

TEST(InputParsing_CheckFileName, EmptyName) {
    EXPECT_FALSE(CheckFileName("", "", false));
}

TEST(InputParsing_CheckFileName, ForbiddenChars) {
    EXPECT_FALSE(CheckFileName("bad:name", "", false));
    EXPECT_FALSE(CheckFileName("bad/name", "", false));
}

TEST(InputParsing_CheckFileName, ForbiddenNames) {
    EXPECT_FALSE(CheckFileName("CON", "", false));
    EXPECT_FALSE(CheckFileName("COM1", "", false));
}

TEST(InputParsing_CheckFileName, ValidNames) {
    EXPECT_TRUE(CheckFileName("myfile.bin", "", false));
    EXPECT_TRUE(CheckFileName("a", "", false));
    EXPECT_TRUE(CheckFileName(string(10, 'x'), "", false));
}

TEST(InputParsing_CheckIfPositiveLong, Empty) {
    EXPECT_FALSE(CheckIfPositiveLong("", false));
}

TEST(InputParsing_CheckIfPositiveLong, Negative) {
    EXPECT_FALSE(CheckIfPositiveLong("-1", false));
}

TEST(InputParsing_CheckIfPositiveLong, Valid) {
    EXPECT_TRUE(CheckIfPositiveLong("0", false));
    EXPECT_TRUE(CheckIfPositiveLong("12345", false));
}

TEST(InputParsing_CheckIfUnsignedShort, Empty) {
    EXPECT_FALSE(CheckIfUnsignedShort("", false));
}

TEST(InputParsing_CheckIfUnsignedShort, Negative) {
    EXPECT_FALSE(CheckIfUnsignedShort("-5", false));
}

TEST(InputParsing_CheckIfUnsignedShort, TooLarge) {
    EXPECT_FALSE(CheckIfUnsignedShort("999999999", false));
}

TEST(InputParsing_CheckIfUnsignedShort, Valid) {
    EXPECT_TRUE(CheckIfUnsignedShort("0", false));
    EXPECT_TRUE(CheckIfUnsignedShort("65535", false));
}
