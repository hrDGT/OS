#include <gtest/gtest.h>
#include <windows.h>
#include <algorithm>
#include <string>
#include "message_queue.h"

struct TestSync
{
  HANDLE hMutex;
  HANDLE hItems;
  HANDLE hSpaces;

  explicit TestSync(int capacity)
  {
    hMutex = CreateMutexA(NULL, FALSE, NULL);
    hItems = CreateSemaphoreA(NULL, 0, capacity, NULL);
    hSpaces = CreateSemaphoreA(NULL, capacity, capacity, NULL);
  }

  ~TestSync()
  {
    CloseHandle(hMutex);
    CloseHandle(hItems);
    CloseHandle(hSpaces);
  }
};

TEST(CreateQueueFile, ReturnsTrue)
{
  EXPECT_TRUE(create_queue_file("tst_create.bin", 4));
  DeleteFileA("tst_create.bin");
}

TEST(CreateQueueFile, FileExists)
{
  create_queue_file("tst_exists.bin", 3);
  HANDLE h = CreateFileA("tst_exists.bin", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  EXPECT_NE(h, INVALID_HANDLE_VALUE);
  if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
  DeleteFileA("tst_exists.bin");
}

TEST(CreateQueueFile, CorrectFileSize)
{
  const int cap = 5;
  create_queue_file("tst_size.bin", cap);

  HANDLE h = CreateFileA("tst_size.bin", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  ASSERT_NE(h, INVALID_HANDLE_VALUE);

  DWORD expected = sizeof(FileHeader) + cap * sizeof(MessageRecord);
  DWORD actual = GetFileSize(h, NULL);
  EXPECT_EQ(actual, expected);

  CloseHandle(h);
  DeleteFileA("tst_size.bin");
}

TEST(CreateQueueFile, OverwritesExistingFile)
{
  create_queue_file("tst_overwrite.bin", 10);
  EXPECT_TRUE(create_queue_file("tst_overwrite.bin", 2));
  HANDLE h = CreateFileA("tst_overwrite.bin", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD expected = sizeof(FileHeader) + 2 * sizeof(MessageRecord);
  EXPECT_EQ(GetFileSize(h, NULL), expected);
  CloseHandle(h);
  DeleteFileA("tst_overwrite.bin");
}

TEST(QueueOps, EnqueueDequeueSingle)
{
  const char* f = "tst_single.bin";
  create_queue_file(f, 3);
  TestSync s(3);

  EXPECT_TRUE(enqueue_message(f, "hello", s.hMutex, s.hSpaces, s.hItems));

  std::string msg;
  EXPECT_TRUE(dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces));
  EXPECT_EQ(msg, "hello");

  DeleteFileA(f);
}

TEST(QueueOps, FIFOOrder)
{
  const char* f = "tst_fifo.bin";
  create_queue_file(f, 5);
  TestSync s(5);

  enqueue_message(f, "first", s.hMutex, s.hSpaces, s.hItems);
  enqueue_message(f, "second", s.hMutex, s.hSpaces, s.hItems);
  enqueue_message(f, "third", s.hMutex, s.hSpaces, s.hItems);

  std::string msg;
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "first");
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "second");
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "third");

  DeleteFileA(f);
}

TEST(QueueOps, WrapAround)
{
  const char* f = "tst_wrap.bin";
  create_queue_file(f, 2);
  TestSync s(2);

  enqueue_message(f, "msg1", s.hMutex, s.hSpaces, s.hItems);
  enqueue_message(f, "msg2", s.hMutex, s.hSpaces, s.hItems);

  std::string msg;
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "msg1");

  enqueue_message(f, "msg3", s.hMutex, s.hSpaces, s.hItems);

  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "msg2");
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "msg3");

  DeleteFileA(f);
}

TEST(QueueOps, MessageTruncation)
{
  const char* f = "tst_trunc.bin";
  create_queue_file(f, 2);
  TestSync s(2);

  std::string longMsg(30, 'x');
  enqueue_message(f, longMsg, s.hMutex, s.hSpaces, s.hItems);

  std::string msg;
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(static_cast<int>(msg.size()), MAX_MESSAGE_LEN - 1);

  DeleteFileA(f);
}

TEST(QueueOps, FillAndDrain)
{
  const int cap = 4;
  const char* f = "tst_full.bin";
  create_queue_file(f, cap);
  TestSync s(cap);

  for (int i = 0; i < cap; i++)
    enqueue_message(f, "item" + std::to_string(i), s.hMutex, s.hSpaces, s.hItems);

  for (int i = 0; i < cap; i++)
  {
    std::string msg;
    dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
    EXPECT_EQ(msg, "item" + std::to_string(i));
  }

  DeleteFileA(f);
}

TEST(QueueOps, InterleaveEnqueueDequeue)
{
  const char* f = "tst_interleave.bin";
  create_queue_file(f, 3);
  TestSync s(3);

  enqueue_message(f, "a", s.hMutex, s.hSpaces, s.hItems);
  enqueue_message(f, "b", s.hMutex, s.hSpaces, s.hItems);

  std::string msg;
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "a");

  enqueue_message(f, "c", s.hMutex, s.hSpaces, s.hItems);
  enqueue_message(f, "d", s.hMutex, s.hSpaces, s.hItems);

  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "b");
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "c");
  dequeue_message(f, msg, s.hMutex, s.hItems, s.hSpaces);
  EXPECT_EQ(msg, "d");

  DeleteFileA(f);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
