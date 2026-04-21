#include "utils/input_parsing.h"
#include "utils/message_queue.h"

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

int main(int argc, char** argv) {
    string file_name;
    cout << "Enter the binary file name:" << endl;
    cin >> file_name;

    while (!CheckFileName(file_name, "", false)) {
        cout << "Enter the binary file name:" << endl;
        cin >> file_name;
    }

    string temp_string;
    cout << "Enter the number of message entries in the binary file (must be > 0):" << endl;
    cin >> temp_string;

    while (!CheckIfPositiveLong(temp_string, false) or stol(temp_string) == 0) {
        cout << "Enter the number of message entries in the binary file (must be > 0):" << endl;
        cin >> temp_string;
    }

    long number_of_entries;
    number_of_entries = stol(temp_string);

    MessageQueue queue(file_name, number_of_entries, true);

    cout << "Enter the number of sender processes (should be >= 0):" << endl;
    cin >> temp_string;

    while (!CheckIfUnsignedShort(temp_string, false)) {
        cout << "Enter the number of sender processes (should be >= 0):" << endl;
        cin >> temp_string;
    }

    unsigned short number_of_senders;
    number_of_senders = static_cast<unsigned short>(stoi(temp_string));

    HANDLE* senders_started_events = new HANDLE[number_of_senders];
    STARTUPINFOA* senders_startup_info = new STARTUPINFOA[number_of_senders];
    PROCESS_INFORMATION* senders_process_info = new PROCESS_INFORMATION[number_of_senders];

    for (unsigned short i = 0; i < number_of_senders; i++) {
        ZeroMemory(&senders_startup_info[i], sizeof(STARTUPINFOA));
        senders_startup_info[i].cb = sizeof(STARTUPINFOA);
        senders_started_events[i] = CreateEventA(NULL, TRUE, FALSE, (string("SENDER_") + to_string(i) + "_STARTED").c_str());

        if (NULL == senders_started_events[i]) {
            cout << "Failed to create start event for sender process " << i << endl;

            return -1;
        }

        string cmd = "Sender.exe " + file_name + " " + to_string(i);

        if (!CreateProcessA(NULL,
            const_cast<char*>(cmd.c_str()),
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &senders_startup_info[i],
            &senders_process_info[i])) {
            cout << "Failed to create sender process " << i << endl;

            return -1;
        }
    }

    WaitForMultipleObjects(number_of_senders, senders_started_events, TRUE, INFINITE);

    cout << "Enter 0 to exit, otherwise enter a number of messages to wait for: ";
    cin >> temp_string;

    while (!CheckIfUnsignedShort(temp_string, false)) {
        cout << "Enter 0 to exit, otherwise enter a number of messages to wait for: ";
        cin >> temp_string;
    }

    unsigned short user_input = static_cast<unsigned short>(stoi(temp_string));

    if (user_input) {
        MessageQueue::Message msg;
        while (true) {
            if (!user_input) {
                cout << "Enter 0 to exit, otherwise enter a number of messages to wait for: ";
                cin >> temp_string;

                while (!CheckIfUnsignedShort(temp_string, false)) {
                    cout << "Enter 0 to exit, otherwise enter a number of messages to wait for: ";
                    cin >> temp_string;
                }

                user_input = static_cast<unsigned short>(stoi(temp_string));

                if (!user_input) {
                    break;
                }
            }

            if (!queue.WDequeue(msg, INFINITE)) {
                cout << "Failed to wait for a message!" << endl;

                return -1;
            }

            cout << "MESSAGE FROM [ " << msg.author << " ]: " << msg.data << endl;

            user_input--;
        }
    }

    delete[] senders_started_events;
    delete[] senders_startup_info;
    delete[] senders_process_info;

    return 0;
}
