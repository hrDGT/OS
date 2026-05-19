#pragma once

#include <windows.h>
#include <cstdint>

#pragma pack(push,1)
struct employee {
    int num;
    char name[10];
    double hours;
};
#pragma pack(pop)

enum OpCode : int {
    OP_READ = 1,
    OP_MODIFY = 2,
    OP_RELEASE = 3,
    OP_EXIT = 4
};


#pragma pack(push,1)
struct Request {
    int op;
    int id;
};
#pragma pack(pop)

#pragma pack(push,1)
struct MsgHeader {
    uint16_t type;
    uint16_t reserved;
    uint32_t length;
};
#pragma pack(pop)
