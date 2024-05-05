#include <iostream>
#include <vector>
#include <sstream>
#include "process_cmd.h"

using namespace std;

/*
 * Returns "should continue" which admittedly should be refactored
 */
bool sanitize_general(vector<string> args) {
    if (args.size() < 2) {
        cout << "All commands must have at least two components, e.g. \"veprom list\"\n";
        return true;
    }
    if (args[0] != "veprom") {
        cout << "All commands must begin with \"veprom\", e.g. \"veprom create 256\"\n";
        return true;
    }

    return false;
}

int main() {
    char ch;

    do {
        int i = 1;
        string input;
        vector<string> args;        
        int f = 1;
        bool soft_continue = false;
        
        cout << "Enter a command for EPROM chip:\n";
        
        getline(cin, input);
        istringstream iss(input);
        string argument;

        while (iss >> argument) {
            args.push_back(argument);
        }

        cout << "user requested:\n";
        for (const string& arg : args) {
            cout << arg << ", ";
        }

        soft_continue = sanitize_general(args);
        if (soft_continue)
            continue;

        // Would have loved to use a switch here but didn't seem worth
        // making a mapping
        cout << "Processing\n";
        if (args[1] == "create") {
            process_create(args);
        }
        else if (args[1] == "load") {
            process_load(args);
        }
        else if (args[1] == "write_raw") {
            process_write_raw(args);
        }
        else if (args[1] == "read_raw") {
            process_read_raw(args);
        }
        else if (args[1] == "write") {
            process_write(args);
        }
        else if (args[1] == "list") {
            process_list(args);
        }
        else if (args[1] == "read") {
            process_read(args);
        }
        else if (args[1] == "erase") {
            process_erase(args);
        }
        else {
            process_help();
        }

    } while (true);

    return 0;
}
