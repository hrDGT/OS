#include <windows.h>
#include <iostream>
#include <string>
#include "common.h"
#include "pipe_utils.h"

using namespace std;

const char* PIPE_NAME = R"(\\.\pipe\LibraryPipe)";

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

int main(int argc, char* argv[]) {
    int clientIndex = 0;

    if (argc >= 2) {
        clientIndex = atoi(argv[1]);
    }

    cout << "Client starting. Index=" << clientIndex << endl;

    HANDLE hPipe = connectToServerPipe(PIPE_NAME, 5000);

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "connectToServerPipe failed\n";

        return 1;
    }

    bool running = true;

    while (running) {
        cout << endl << "Choose operation:" << endl << "1 - Read record"<< endl
            << "2 - Modify record" << endl << "3 - Exit" << endl << " > ";

        int choice;
        cin >> choice;

        if (choice == 3) {
            Request r{ OP_EXIT, 0 };
            sendWithHeader(hPipe, 1, &r, sizeof(r));
            running = false;

            break;
        }
        else if (choice == 1) {
            cout << "Enter employee id to read: ";
            int id;
            cin >> id;

            Request r{ OP_READ, id };
            sendWithHeader(hPipe, 1, &r, sizeof(r));

            MsgHeader h;

            if (!readHeader(hPipe, h)) {
                cerr << "Read header failed" << endl;
                break;
            }

            if (h.type == 2 and h.length == sizeof(employee)) {
                employee e;

                if (!readPayload(hPipe, &e, sizeof(e))) {
                    cerr << "Read payload failed" << endl;

                    break;
                }

                if (e.num < 0) {
                    cout << "Record not found or error (code=" << e.num << ")" << endl;
                }
                else {
                    cout << "Received: id=" << e.num << " name=" << e.name << " hours=" << e.hours << endl;
                }
            }
            else {
                cout << "Unexpected response\n";
            }

            cout << "Press Enter to release record...";
            cin.ignore();
            cin.get();

            Request rel{ OP_RELEASE, id };
            sendWithHeader(hPipe, 1, &rel, sizeof(rel));
        }
        else if (choice == 2) {
            cout << "Enter employee id to modify: ";
            int id;
            cin >> id;

            Request r{ OP_MODIFY, id };
            sendWithHeader(hPipe, 1, &r, sizeof(r));

            MsgHeader h;

            if (!readHeader(hPipe, h)) {
                cerr << "Read header failed" << endl;

                break;
            }
            if (h.type == 2 and h.length == sizeof(employee)) {
                employee e;

                if (!readPayload(hPipe, &e, sizeof(e))) {
                    cerr << "Read payload failed" << endl;

                    break;
                }

                if (e.num < 0) {
                    cout << "Record not found or error (code=" << e.num << ")" << endl;

                    continue;
                }

                cout << "Current: id=" << e.num << " name=" << e.name << " hours=" << e.hours << endl;

                string newname;
                double newhours;
                cout << "Enter new name: ";
                cin >> newname;
                cout << "Enter new hours: ";
                cin >> newhours;

                employee ne = e;
                memset(ne.name, 0, sizeof(ne.name));
                strncpy_s(ne.name, newname.c_str(), sizeof(ne.name) - 1);
                ne.hours = newhours;

                Request sendMod{ OP_MODIFY, id };
                sendWithHeader(hPipe, 1, &sendMod, sizeof(sendMod));
                sendWithHeader(hPipe, 2, &ne, sizeof(ne));

                MsgHeader confH;
                if (readHeader(hPipe, confH) && confH.type == 3 && confH.length == sizeof(employee)) {
                    employee conf;

                    if (readPayload(hPipe, &conf, sizeof(conf))) {
                        cout << "Server confirmed: id=" << conf.num << " name=" << conf.name << " hours=" << conf.hours << endl;
                    }
                }

                cout << "Press Enter to release record...";
                cin.ignore();
                cin.get();
                Request rel{ OP_RELEASE, id };
                sendWithHeader(hPipe, 1, &rel, sizeof(rel));
            }
            else {
                cout << "Unexpected response\n";
            }
        }
        else {
            cout << "Unknown choice\n";
        }
    }

    CloseHandle(hPipe);
    cout << "Client exiting.\n";
    return 0;
}
