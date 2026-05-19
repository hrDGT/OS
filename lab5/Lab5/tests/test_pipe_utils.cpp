#include <gtest/gtest.h>
#include <thread>
#include "../pipe_utils.h"

const char* TEST_PIPE = R"(\\.\pipe\TestPipeForGTest)";

TEST(PipeUtils, CreateAndConnect) {
    HANDLE server = createServerPipeInstance(TEST_PIPE);
    ASSERT_NE(server, INVALID_HANDLE_VALUE);

    std::thread clientThread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        HANDLE c = CreateFileA(TEST_PIPE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (c != INVALID_HANDLE_VALUE) {
            CloseHandle(c);
        }
        });

    bool ok = waitForClientConnect(server);
    EXPECT_TRUE(ok);

    if (server != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(server);
        DisconnectNamedPipe(server);
        CloseHandle(server);
    }

    clientThread.join();
}
