

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include "Emulator.h"

using namespace std;

enum command { CREATE = 1, LOAD, READ_RAW, WRITE_RAW, WRITE, READ, LIST, ERASE };
unordered_map<string, command> commands_list = { { "create", CREATE},
    {"load", LOAD}, {"read_raw", READ_RAW},
    {"write_raw", WRITE_RAW}, {"write", WRITE},
    {"read", READ}, {"list", LIST},
    {"erase", ERASE} };

int main()
{

    Emulator *Em = new Emulator();
    while (1) {
        string com, field1, field2, field3;
        getline(cin, com);

        stringstream X(com);
        getline(X, field1, ' ');
        getline(X, field2, ' ');
        getline(X, field3, ' ');

        int command = commands_list[field1];

        //create, load, read raw, write raw, write, read, list, erase
        // lots of ways to make it safe to handle corner cases but, but..
        switch (command)
        {
        case CREATE:
            Em->create_Veprom(stoi(field2));
            break;

        case LOAD:
            if (Em->load_File(field2)) {
                cout << "loaded" << endl;
            }
            break;

        case READ_RAW:
            Em->read_from_Veprom(stol(field2), stoi(field3));
            break;

        case WRITE_RAW:
            Em->write_to_Veprom(stol(field2), field3.c_str());
            break;

        case WRITE:
            Em->write_file_to_Veprom(field2);
            break;

        case READ:
            Em->read_file_from_Veprom(field2);
            break;

        case LIST:
            Em->list_Files();
            break;

        case ERASE:
            Em->erase_Veprom();
            break;

        default:
            cout << "Invalid command" << endl;
            break;

        }
    }
}
