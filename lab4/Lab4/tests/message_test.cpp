#include "../utils/message_queue.h"

#include <gtest/gtest.h>
#include <string>
#include <cstring>

using namespace std;

TEST(MessageStruct, DefaultConstructor) {
    MessageQueue::Message m;
    EXPECT_EQ(m.author, 0);
    EXPECT_EQ(m.data[0], '\0');
}

TEST(MessageStruct, FromStringShort) {
    string s = "hello";
    MessageQueue::Message m(s, 7);
    EXPECT_EQ(m.author, 7);
    EXPECT_STREQ(m.data, "hello");
}

TEST(MessageStruct, FromStringLongTruncate) {
    string longs(30, 'A');
    MessageQueue::Message m(longs, 3);
    EXPECT_EQ(m.author, 3);
    EXPECT_EQ(m.data[19], '\0');
    for (size_t i = 0; i < 19; ++i) {
        EXPECT_EQ(m.data[i], 'A');
    }
}

TEST(InfoStruct, Defaults) {
    MessageQueue::Info info;
    EXPECT_EQ(info.capacity, 0);
    EXPECT_EQ(info.size, 0);
    EXPECT_EQ(info.front, 0);

    MessageQueue::Info info2(10);
    EXPECT_EQ(info2.capacity, 10);
    EXPECT_EQ(info2.size, 0);
    EXPECT_EQ(info2.front, 0);
}
