
#include <iostream>

#include "write_file_command.h"
#include "virtual_eprom.h"

int WriteFileCommand::run(Context& context, int argc, char* argv[]) {

    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        displayHelp();
        return 0;
    }

    auto filename = context.readCurrentVepromFile();
    std::string dataFile = argv[1];

    try {
        VirtualEprom eprom{filename};
        eprom.writeFile(dataFile);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}

std::string WriteFileCommand::getDescription() {
    return "Writes the FILE to the virtual EPROM chip";
}

std::string WriteFileCommand::getName() {
    return "write";
}

void WriteFileCommand::displayHelp() {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << getName() << " FILE" << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}
