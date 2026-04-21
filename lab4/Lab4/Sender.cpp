#include "utils/input_parsing.h"
#include "utils/message_queue.h"

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Too few arguments passed!" << endl;

        return -1;
    }

    string file_name = argv[1];
    unsigned short sender_num = static_cast<unsigned short>(atoi(argv[2]));

    HANDLE process_start_event = OpenEventA(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, (string("SENDER_") + argv[2] + "_STARTED").c_str());

    if (process_start_event == NULL) {
        cout << "Failed to open start event!" << endl;

        return -1;
    }

    SetEvent(process_start_event);

    string temp_string;
    cout << "Enter 0 to exit, otherwise enter a number of messages to send: ";
    cin >> temp_string;

    while (!CheckIfUnsignedShort(temp_string, false)) {
        cout << "Enter 0 to exit, otherwise enter a number of messages to send: ";
        cin >> temp_string;
    }

    unsigned short user_input = static_cast<unsigned short>(stoi(temp_string));

    if (user_input) {
        MessageQueue queue(file_name, 0l, false);
        MessageQueue::Message msg;

        while (true) {
            if (!user_input) {
                cout << "Enter 0 to exit, otherwise enter a number of messages to send: ";
                cin >> temp_string;

                while (!CheckIfUnsignedShort(temp_string, false)) {
                    cout << "Enter 0 to exit, otherwise enter a number of messages to send: ";
                    cin >> temp_string;
                }

                user_input = static_cast<unsigned short>(stoi(temp_string));

                if (!user_input) {
                    break;
                }
            }

            cout << "--> ";
            cin >> temp_string;

            if (temp_string.length() > 19) {
                cout << "Message too long, will send only the first 19 characters!" << endl;
            }

            msg = MessageQueue::Message(temp_string, sender_num);

            if (!queue.WEnqueue(msg, INFINITE)) {
                cout << "Failed to wait for a free space in the message queue!" << endl;

                return -1;
            }

            user_input--;
        }
    }

    return 0;
}
