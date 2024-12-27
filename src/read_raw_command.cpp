
#include <iostream>

#include "read_raw_command.h"
#include "virtual_eprom.h"

int ReadRawCommand::run(Context& context, int argc, char* argv[]) {

    if (argc < 3 || strcmp(argv[1], "help") == 0) {
        displayHelp();
        return 0;
    }

    auto filename = context.readCurrentVepromFile();
    ulong address = strtoul(argv[1], NULL, 0);
    ulong length = strtoul(argv[2], NULL, 0);

    try {
        VirtualEprom eprom{filename};
        auto data = eprom.readRaw(address, length);
        std::cout << data << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}

std::string ReadRawCommand::getDescription() {
    return "Reads the values stored at a specific ADDRESS and LENGTH on the virtual EPROM chip and outputs it on stdout";
}

std::string ReadRawCommand::getName() {
    return "read_raw";
}

void ReadRawCommand::displayHelp() {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << getName() << " ADDRESS LENGTH" << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}

