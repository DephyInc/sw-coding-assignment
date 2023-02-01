#pragma once

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;


class Localization {
    public:
        const static inline char* NO_FILES_FOUND_MSG = "No files detected in vEPROM";
        const static inline char* INVALID_CMD_ARGS_MSG = "Invalid command/arguments specified!";
};


class Utils {
    public:
        const static inline int bytesInKB = 1024;
        
        static bool isFileExists(string fileName) {
            ifstream file(fileName.c_str());
            bool result = file.good();
            file.close();
            return result;
        }
        
        static int convertStringHexToInt(string hexInput) {

            if(!hasHexDigits(hexInput)) {
                return stoi(hexInput);
            }

            stringstream ss;
            ss << hexInput;
            
            int decimal;
            ss >> hex >> decimal;

            return decimal;
        }

        static bool isWithinRange(int n, int s, int e) {
            return n >= s && n <= e;
        }

        static bool hasHexDigits(string val) {
            ostringstream output;
            output << hex << val;
            return output.str().find_first_of("abcdefABCDEF") != string::npos;
        }
};