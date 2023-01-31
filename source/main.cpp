#include <iostream>
#include "veprom.h"

#define USAGE \
    "Usages:\n" \
    " >> veprom " METHOD_CREATE " [size in KB (int)] \n" \
    " >> ... "

#define __check_input(condition, error) \
    if (!(condition)) \
    {cout << USAGE; return error;}

#define __check_app(condition, error) \
    if (!(condition)) \
    {cout << "ERROR: " << error; return error;}

int main(int argc, char* argv[])
{
    __check_input(argc > 1, Veprom::RequireMethod);

    // Create vEPROM instance
    Veprom veprom = Veprom();
    Veprom::eRetVal ret = Veprom::__Unknown__;

    // Method: "create"
    if (strcmp(argv[1], METHOD_CREATE) == 0)
    {
        // Get vEPROM size (limit to 1024 KB)
        __check_input(argc > 2, Veprom::RequireSize);
        int szKB = atoi(argv[2]);
        __check_input(szKB >= 0, Veprom::InvalidSize);
        __check_input(szKB <= 1024, Veprom::SizeTooLarge);

        // Create vEPROM and output filename
        string filename;
        Veprom::eRetVal ret = veprom.create(szKB * 1024, filename);
        __check_app(ret == Veprom::OK, ret);
        cout << filename;
        return 0;
    }
    else if (strcmp(argv[1], METHOD_LOAD) == 0)
    {
        // Get choice of file to load
        __check_input(argc > 2, Veprom::RequireLoadChoice);
        string filename = argv[2];
        
        // Setup choice
        Veprom::eRetVal ret = veprom.load(filename);
        __check_app(ret == Veprom::OK, ret);
        return 0;
    }
    else if (strcmp(argv[1], METHOD_WRITE_RAW) == 0)
    {
        // Get address and payload
        __check_input(argc > 2, Veprom::RequireAddress);
        // -----------------------------------------------
        // TODO: differentiate addr = 0 and addr = invalid
        // -----------------------------------------------
        int addr = atoi(argv[2]); 
        __check_input(argc > 3, Veprom::RequireData);
        string data = argv[3];

        // Execute write raw
        Veprom::eRetVal ret = veprom.write_raw(addr, data);
        __check_app(ret == Veprom::OK, ret);
        return 0;
    }
    else
    {   // Method not found
        __check_input(false, Veprom::InvalidMethod);
    }
}