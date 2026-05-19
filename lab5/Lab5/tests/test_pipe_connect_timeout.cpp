#include <gtest/gtest.h>
#include "../pipe_utils.h"
#include "windows.h"

const char* TIMEOUT_PIPE = R"(\\.\pipe\TimeoutPipeForGTest)";

TEST(PipeUtils, ConnectTimeout) {
    HANDLE h = connectToServerPipe(TIMEOUT_PIPE, 500); // 500 ms
    EXPECT_EQ(h, INVALID_HANDLE_VALUE);
}
