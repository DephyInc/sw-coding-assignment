#include <iostream>
#include "veprom.h"

#define USAGE \
    "Usages:\n" \
    " >> veprom " METHOD_CREATE " [size in KB] \n" \
    " >> ... "

#define __check_input(condition, error) \
    if (!(condition)) \
    {cout << USAGE; return error;}

#define __check_runtime(condition, error) \
    if (!(condition)) \
    {cout << "ERROR: " << error; return error;}

int main(int argc, char* argv[])
{
    __check_input(argc > 1, Veprom::RequireMethod);

    // Create vEPROM instance
    Veprom veprom = Veprom();

    // Method: "create"
    if (strcmp(argv[1], METHOD_CREATE) == 0)
    {
        // Get vEPROM size (limit to 1024 KB)
        __check_input(argc > 2, Veprom::RequireSize);
        int szKB = atoi(argv[2]);
        __check_input(szKB != 0, Veprom::InvalidSize);
        __check_input(szKB <= 1024, Veprom::SizeTooLarge);

        // Create vEPROM and output filename
        string filename;
        Veprom::eRetVal ret = veprom.create(szKB * 1024, filename);
        __check_runtime(ret == Veprom::OK, ret);
        cout << filename;
    }
    return 0;
}