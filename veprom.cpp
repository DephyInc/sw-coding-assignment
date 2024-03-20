#include "fileHandler.h"


/// @brief Implementation of VEPROM By: Arash Agan 
/*
    * Video demonstration is uploaded to youtube https://youtu.be/nkI_sSfGi3E
    * Structure of virtual EPROM file

    - Number of files in the eprom (4 bytes)

    - Name of file1
    - Size of file1 (4 bytes)
    - Contents of file1

    - Name of file2
    - Size of file2 (4 bytes)
    - Contents of file2
*/
int main(int argc, char *argv[])
{
    fileHandler handler{};
    handler.parseCommandlineArguments(argc, argv);

    return 0;
}
