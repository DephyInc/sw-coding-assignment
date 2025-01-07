
#include <iostream>
#include <vector>

#include "write_raw_command.h"
#include "virtual_eprom.h"

int WriteRawCommand::run(Context& context, int argc, char* argv[]) {

    if (argc < 3 || strcmp(argv[1], "help") == 0) {
        displayHelp();
        return 0;
    }

    auto filename = context.readCurrentVepromFile();
    ulong address = strtoul(argv[1], NULL, 0);
    std::string data = argv[2];
    
    try {
        VirtualEprom eprom{filename};
        eprom.writeRaw(address, data);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}

std::string WriteRawCommand::getDescription() {
    return "Writes a raw STRING of bytes to a specific ADDRESS on the virtual EPROM chip";
}

std::string WriteRawCommand::getName() {
    return "write_raw";
}

void WriteRawCommand::displayHelp() {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << getName() << " ADDRESS STRING" << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}
