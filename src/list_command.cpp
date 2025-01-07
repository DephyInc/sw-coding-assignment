
#include <iostream>
#include <iomanip>

#include "list_command.h"
#include "virtual_eprom.h"

int ListCommand::run(Context& context, int argc, char* argv[]) {

    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        displayHelp(argv[0]);
        return 0;
    }
    
    auto filename = context.readCurrentVepromFile();
    
    VirtualEprom eprom{filename};
    auto capacity = eprom.getCapacity();
    auto files = eprom.listFiles();

    // Print vEPROM capacity to stdout
    printf("capacity %ld\n", capacity);

    // Print file info to stdout
    for (const FileInfo& f : files) {
        printf("%08X %11d  %s\n", f.offset, f.size, f.filename.c_str());
    }

    return 0;
}

std::string ListCommand::getDescription() {
    return "Lists the files on the virtual EPROM chip";
}

std::string ListCommand::getName() {
    return "list";
}

void ListCommand::displayHelp(char* commandString) {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << commandString << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}
