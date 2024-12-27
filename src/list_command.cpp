
#include <iostream>

#include "list_command.h"
#include "virtual_eprom.h"

int ListCommand::run(Context& context, int argc, char* argv[]) {

    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        displayHelp(argv[0]);
        return 0;
    }
    
    auto filename = context.readCurrentVepromFile();
    
    VirtualEprom eprom{filename};
    eprom.listFiles();
    
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
