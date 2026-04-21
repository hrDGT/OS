#include "message_queue.h"
#include <algorithm>
#include <cstring>

static HANDLE open_file_rw(const std::string& filename)
{
  return CreateFileA(
    filename.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );
}

bool create_queue_file(const std::string& filename, int capacity)
{
  HANDLE hFile = CreateFileA(
    filename.c_str(),
    GENERIC_WRITE,
    0, NULL,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );
  if (hFile == INVALID_HANDLE_VALUE) return false;

  FileHeader header{0, 0, capacity, 0};
  DWORD written;
  if (!WriteFile(hFile, &header, sizeof(FileHeader), &written, NULL))
  {
    CloseHandle(hFile);
    return false;
  }

  MessageRecord empty{};
  for (int i = 0; i < capacity; i++)
    WriteFile(hFile, &empty, sizeof(MessageRecord), &written, NULL);

  CloseHandle(hFile);
  return true;
}

bool enqueue_message(const std::string& filename, const std::string& msg, HANDLE hMutex, HANDLE hSpaces, HANDLE hItems)
{
  if (WaitForSingleObject(hSpaces, INFINITE) != WAIT_OBJECT_0) return false;

  if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0)
  {
    ReleaseSemaphore(hSpaces, 1, NULL);
    return false;
  }

  HANDLE hFile = open_file_rw(filename);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ReleaseMutex(hMutex);
    ReleaseSemaphore(hSpaces, 1, NULL);
    return false;
  }

  FileHeader header;
  DWORD bytes;
  ReadFile(hFile, &header, sizeof(FileHeader), &bytes, NULL);

  DWORD offset = static_cast<DWORD>(sizeof(FileHeader) + header.tail * sizeof(MessageRecord));
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);

  MessageRecord record{};
  size_t len = msg.size();
  if (len > MAX_MESSAGE_LEN - 1) len = MAX_MESSAGE_LEN - 1;
  memcpy(record.text, msg.c_str(), len);

  WriteFile(hFile, &record, sizeof(MessageRecord), &bytes, NULL);

  header.tail = (header.tail + 1) % header.capacity;
  header.count++;
  SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
  WriteFile(hFile, &header, sizeof(FileHeader), &bytes, NULL);

  CloseHandle(hFile);
  ReleaseMutex(hMutex);

  ReleaseSemaphore(hItems, 1, NULL);
  return true;
}

bool dequeue_message(const std::string& filename, std::string& out_msg, HANDLE hMutex, HANDLE hItems, HANDLE hSpaces)
{
  if (WaitForSingleObject(hItems, INFINITE) != WAIT_OBJECT_0) return false;

  if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0)
  {
    ReleaseSemaphore(hItems, 1, NULL);
    return false;
  }

  HANDLE hFile = open_file_rw(filename);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ReleaseMutex(hMutex);
    ReleaseSemaphore(hItems, 1, NULL);
    return false;
  }

  FileHeader header;
  DWORD bytes;
  ReadFile(hFile, &header, sizeof(FileHeader), &bytes, NULL);

  DWORD offset = static_cast<DWORD>(sizeof(FileHeader) + header.head * sizeof(MessageRecord));
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);

  MessageRecord record;
  ReadFile(hFile, &record, sizeof(MessageRecord), &bytes, NULL);
  out_msg = std::string(record.text);

  header.head = (header.head + 1) % header.capacity;
  header.count--;
  SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
  WriteFile(hFile, &header, sizeof(FileHeader), &bytes, NULL);

  CloseHandle(hFile);
  ReleaseMutex(hMutex);

  ReleaseSemaphore(hSpaces, 1, NULL);
  return true;
}
