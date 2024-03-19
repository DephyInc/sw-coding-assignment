#include <iostream>
#include <fstream>
#include <string>
#include "fileHandler.h"

int main(int argc, char *argv[])
{
    fileHandler handler{};
    handler.parseCommandlineArguments(argc, argv);

    return 0;
}
