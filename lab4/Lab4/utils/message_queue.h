#pragma once

#include <string>
#include <windows.h>
#include <fstream>
#include <stdexcept>
#include <cstdio>

class MessageQueue {
public:
    struct Message {
        char data[20];
        unsigned short author;

        Message() {
            data[0] = '\0';
            author = 0;
        }

        Message(const std::string& s, unsigned short a) {
            author = a;
            size_t len = s.copy(data, sizeof(data) - 1);
            data[len] = '\0';
        }
    };

    struct Info {
        LONG capacity;
        LONG size;
        LONG front;

        Info() : capacity(0), size(0), front(0) {}
        Info(LONG cap) : capacity(cap), size(0), front(0) {}
    };

    MessageQueue(std::string _file_name, LONG number_of_entries, bool own = true);
    ~MessageQueue();

    bool WEnqueue(Message message, DWORD wait_for = INFINITE);
    bool WDequeue(Message& destination, DWORD wait_for = INFINITE);

private:
    std::string file_name;
    bool is_owner;

    HANDLE enq_semaphore = NULL;
    HANDLE deq_semaphore = NULL;
    HANDLE file_mutex = NULL;
};
