#include <gtest/gtest.h>
#include "../proc_utils.h"
#include <vector>

TEST(ProcUtils, LaunchClientsMissingExe) {
    std::string fake = "nonexistent_executable_12345.exe";
    auto procs = launchClients(2, fake);
    EXPECT_TRUE(procs.empty());
}
