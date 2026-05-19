#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <conio.h>
#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "common.h"
#include "file_utils.h"
#include "lock_utils.h"
#include "pipe_utils.h"
#include "proc_utils.h"

using namespace std;

const char* PIPE_NAME = R"(\\.\pipe\LibraryPipe)";
const char* DATA_FILE = "employees.dat";

static unordered_map<int, shared_ptr<shared_mutex>> recordLocks;
static mutex locksMapMutex;

static shared_ptr<shared_mutex> getLockForId(int id) {
    lock_guard<mutex> g(locksMapMutex);
    auto it = recordLocks.find(id);

    if (it != recordLocks.end()) {
        return it->second;
    }

    auto m = make_shared<shared_mutex>();
    recordLocks[id] = m;

    return m;
}

void sendWithHeader(HANDLE hPipe, uint16_t type, const void* payload, uint32_t length) {
    MsgHeader h = { type, 0, length };
    DWORD written;
    WriteFile(hPipe, &h, sizeof(h), &written, NULL);

    if (length and payload) {
        WriteFile(hPipe, payload, length, &written, NULL);
    }
}

bool readHeader(HANDLE hPipe, MsgHeader& h) {
    DWORD read;

    if (!ReadFile(hPipe, &h, sizeof(h), &read, NULL) or read != sizeof(h)) {
        return false;
    }

    return true;
}

bool readPayload(HANDLE hPipe, void* buf, uint32_t length) {
    DWORD read;
    uint32_t total = 0;
    char* p = (char*)buf;

    while (total < length) {
        if (!ReadFile(hPipe, p + total, length - total, &read, NULL) or read == 0) {
            return false;
        }

        total += read;
    }

    return true;
}

void handleClient(HANDLE hPipe) {
    HANDLE hFile = CreateFileA(DATA_FILE, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "CreateFileA in thread failed: " << GetLastError() << "\n";
        CloseHandle(hPipe);

        return;
    }

    while (true) {
        MsgHeader hdr;

        if (!readHeader(hPipe, hdr)) {
            break;
        }

        if (hdr.type != 1 or hdr.length != sizeof(Request)) {
            break;
        }

        Request req;

        if (!readPayload(hPipe, &req, sizeof(req))) {
            break;
        }

        if (req.op == OP_EXIT) {
            break;
        }

        LARGE_INTEGER offset;

        if (!findRecordOffset(DATA_FILE, req.id, offset)) {
            employee notfound = { -1, "", 0.0 };
            sendWithHeader(hPipe, 4, &notfound, sizeof(notfound));

            continue;
        }

        if (req.op == OP_READ) {
            auto mtx = getLockForId(req.id);
            shared_lock<shared_mutex> guard(*mtx);

            employee e;

            if (!readRecordAtOffset(hFile, offset, e)) {
                employee err = { -2, "", 0.0 };
                sendWithHeader(hPipe, 4, &err, sizeof(err));

                continue;
            }

            sendWithHeader(hPipe, 2, &e, sizeof(e));

            MsgHeader relh;

            if (!readHeader(hPipe, relh)) {
                break;
            }

            if (relh.type == 1 and relh.length == sizeof(Request)) {
                Request rel;

                if (!readPayload(hPipe, &rel, sizeof(rel))) {
                    break;
                }
            }
            else {
                unlockRange(hFile, offset);
            }
        }
        else if (req.op == OP_MODIFY) {
            auto mtx = getLockForId(req.id);
            unique_lock<shared_mutex> guard(*mtx);

            employee e;

            if (!readRecordAtOffset(hFile, offset, e)) {
                employee err = { -2, "", 0.0 };
                sendWithHeader(hPipe, 4, &err, sizeof(err));

                continue;
            }

            sendWithHeader(hPipe, 2, &e, sizeof(e));

            MsgHeader nextH;

            if (!readHeader(hPipe, nextH)) {
                break;
            }

            if (nextH.type == 1 and nextH.length == sizeof(Request)) {
                Request nextReq;

                if (!readPayload(hPipe, &nextReq, sizeof(nextReq))) {
                    break;
                }

                if (nextReq.op == OP_MODIFY) {
                    MsgHeader recH;

                    if (!readHeader(hPipe, recH)) {
                        break;
                    }

                    if (recH.type == 2 and recH.length == sizeof(employee)) {
                        employee ne;

                        if (!readPayload(hPipe, &ne, sizeof(ne))) {
                            break;
                        }

                        if (!writeRecordAtOffset(hFile, offset, ne)) {
                            employee err = { -2, "", 0.0 };
                            sendWithHeader(hPipe, 4, &err, sizeof(err));

                            continue;
                        }

                        sendWithHeader(hPipe, 3, &ne, sizeof(ne));
                        MsgHeader relH;

                        if (!readHeader(hPipe, relH)) {
                            break;
                        }

                        if (relH.type == 1 and relH.length == sizeof(Request)) {
                            Request rel;

                            if (!readPayload(hPipe, &rel, sizeof(rel))) {
                                break;
                            }
                        }
                    }
                    else {
                        cerr << "Expected employee payload but got different header" << endl;
                    }
                }
                else if (nextReq.op == OP_RELEASE) {

                }
                else {
                    cerr << "Unexpected request after MODIFY: op=" << nextReq.op << endl;
                }
            }
            else {
                cerr << "Unexpected header after sending record (expected Request)" << endl;
            }
        }
        else {
            employee err = { -3, "", 0.0 };
            sendWithHeader(hPipe, 4, &err, sizeof(err));
        }
    }

    CloseHandle(hFile);
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

int main() {
    cout << "Server starting..." << endl;

    int n;
    cout << "Enter number of employees to create: ";
    cin >> n;

    vector<employee> v;

    for (int i = 0; i < n; ++i) {
        employee e;
        cout << "Employee #" << i + 1 << " id: ";
        cin >> e.num;

        string name;
        cout << "name: ";
        cin >> name;

        memset(e.name, 0, sizeof(e.name));
        strncpy_s(e.name, name.c_str(), sizeof(e.name) - 1);
        cout << "hours: ";
        cin >> e.hours;
        v.push_back(e);
    }

    if (!writeEmployeesToFile(DATA_FILE, v)) {
        return 1;
    }

    printFileContents(DATA_FILE);

    int clientCount = 0;
    cout << "Enter number of client processes to launch: ";
    cin >> clientCount;

    vector<HANDLE> pipes;
    pipes.reserve(clientCount);

    for (int i = 0; i < clientCount; i++) {
        HANDLE h = createServerPipeInstance(PIPE_NAME);

        if (h == INVALID_HANDLE_VALUE) {
            cerr << "Failed to create pipe instance #" << i + 1 << endl;

            for (HANDLE ph : pipes) {
                if (ph != INVALID_HANDLE_VALUE) {
                    CloseHandle(ph);
                }
            }

            return 1;
        }

        pipes.push_back(h);
    }

    string clientPath = "Client.exe";
    auto procs = launchClients(clientCount, clientPath);

    int connectedClients = 0;

    for (size_t i = 0; i < pipes.size(); ++i) {
        HANDLE hPipe = pipes[i];

        cout << "Waiting for client to connect (pipe #" << i + 1 << ")..." << endl;

        if (!waitForClientConnect(hPipe)) {
            cerr << "Client did not connect to pipe #" << i + 1 << endl;

            if (hPipe != INVALID_HANDLE_VALUE) {
                CloseHandle(hPipe);
            }

            pipes[i] = INVALID_HANDLE_VALUE;

            continue;
        }

        cout << "Client connected on pipe #" << i + 1 << ", spawning thread..." << endl;
        thread t(handleClient, hPipe);
        t.detach();
        pipes[i] = INVALID_HANDLE_VALUE;
        connectedClients++;
    }

    for (HANDLE ph : pipes) {
        if (ph != INVALID_HANDLE_VALUE) {
            CloseHandle(ph);
        }
    }

    if (!procs.empty()) {
        waitForClientsAndClose(procs);
    }

    printFileContents(DATA_FILE);

    return 0;
}
