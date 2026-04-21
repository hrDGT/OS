#pragma once
#include <windows.h>
#include <string>

constexpr int MAX_MESSAGE_LEN = 20;

constexpr const char* MUTEX_NAME = "MsgQueueMutex";
constexpr const char* ITEMS_SEM_NAME = "MsgQueueItems";
constexpr const char* SPACES_SEM_NAME = "MsgQueueSpaces";
constexpr const char* READY_SEM_NAME = "MsgQueueReady";

struct FileHeader
{
  int head;
  int tail;
  int capacity;
  int count;
};

struct MessageRecord
{
  char text[MAX_MESSAGE_LEN];
};

bool create_queue_file(const std::string& filename, int capacity);

bool enqueue_message(const std::string& filename, const std::string& msg, HANDLE hMutex, HANDLE hSpaces, HANDLE hItems);

bool dequeue_message(const std::string& filename, std::string& out_msg, HANDLE hMutex, HANDLE hItems, HANDLE hSpaces);
