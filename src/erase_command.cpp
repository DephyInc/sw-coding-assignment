
#include "erase_command.h"
#include "virtual_eprom.h"

int EraseCommand::run(Context& context, int argc, char* argv[]) {
    
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        displayHelp(argv[0]);
        return 0;
    }
    
    auto filename = context.readCurrentVepromFile();
    
    VirtualEprom eprom{filename};
    eprom.erase();
    
    return 0;
}

std::string EraseCommand::getDescription() {
    return "Sets the EPROM back to its original state";
}

std::string EraseCommand::getName() {
    return "erase";
}

void EraseCommand::displayHelp(char* commandString) {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << commandString << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}
