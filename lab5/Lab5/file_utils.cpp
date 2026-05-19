#include "file_utils.h"
#include <iostream>
#include <cstring>

bool writeEmployeesToFile(const char* filename, const std::vector<employee>& v) {
    HANDLE h = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFileA failed: " << GetLastError() << "\n";

        return false;
    }

    DWORD written;

    if (!WriteFile(h, v.data(), (DWORD)(v.size() * sizeof(employee)), &written, NULL) or written != v.size() * sizeof(employee)) {
        std::cerr << "WriteFile failed: " << GetLastError() << "\n";
        CloseHandle(h);

        return false;
    }

    CloseHandle(h);

    return true;
}

bool printFileContents(const char* filename) {
    HANDLE h = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h == INVALID_HANDLE_VALUE) {
        std::cerr << "Open file for print failed: " << GetLastError() << "\n";

        return false;
    }

    LARGE_INTEGER size;

    if (!GetFileSizeEx(h, &size)) {
        std::cerr << "GetFileSizeEx failed: " << GetLastError() << "\n";
        CloseHandle(h);

        return false;
    }

    size_t count = (size_t)size.QuadPart / sizeof(employee);
    std::vector<employee> buf(count);
    DWORD read;

    if (!ReadFile(h, buf.data(), (DWORD)(count * sizeof(employee)), &read, NULL) or read != count * sizeof(employee)) {
        std::cerr << "ReadFile failed: " << GetLastError() << "\n";
        CloseHandle(h);

        return false;
    }

    std::cout << "=== File contents (" << count << " records) ===\n";

    for (size_t i = 0; i < count; i++) {
        std::cout << "num=" << buf[i].num << " name=" << buf[i].name << " hours=" << buf[i].hours << "\n";
    }

    CloseHandle(h);

    return true;
}

bool findRecordOffset(const char* filename, int id, LARGE_INTEGER& offset) {
    HANDLE h = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h == INVALID_HANDLE_VALUE) {
        std::cerr << "Open file for find failed: " << GetLastError() << "\n";
        return false;
    }

    DWORD read;
    employee e;
    LARGE_INTEGER pos; pos.QuadPart = 0;

    while (ReadFile(h, &e, sizeof(employee), &read, NULL) and read == sizeof(employee)) {
        if (e.num == id) {
            offset = pos;
            CloseHandle(h);

            return true;
        }

        pos.QuadPart += sizeof(employee);
    }

    CloseHandle(h);

    return false;
}

bool readRecordAtOffset(HANDLE hFile, const LARGE_INTEGER& offset, employee& out) {
    if (!SetFilePointerEx(hFile, offset, NULL, FILE_BEGIN)) {
        std::cerr << "SetFilePointerEx failed: " << GetLastError() << "\n";

        return false;
    }

    DWORD rd;

    if (!ReadFile(hFile, &out, sizeof(out), &rd, NULL) or rd != sizeof(out)) {
        std::cerr << "ReadFile record failed: " << GetLastError() << "\n";

        return false;
    }

    return true;
}

bool writeRecordAtOffset(HANDLE hFile, const LARGE_INTEGER& offset, const employee& in) {
    if (!SetFilePointerEx(hFile, offset, NULL, FILE_BEGIN)) {
        std::cerr << "SetFilePointerEx failed: " << GetLastError() << "\n";

        return false;
    }

    DWORD wr;

    if (!WriteFile(hFile, &in, sizeof(in), &wr, NULL) or wr != sizeof(in)) {
        std::cerr << "WriteFile record failed: " << GetLastError() << "\n";

        return false;
    }

    return true;
}
