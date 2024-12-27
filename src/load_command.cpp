
#include <iostream>

#include "load_command.h"

int LoadCommand::run(Context& context, int argc, char* argv[]) {

    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        displayHelp(argv[0]);
        return -1;
    }
    
    // Check if file exists
    std::ifstream infile(argv[1]);        
    if (infile) {
        context.saveCurrentVepromFile(argv[1]);
    } else {
        std::cerr << "ERROR: vEPROM file does not exist" << std::endl;
        return -1;
    }
    
    return 0;
}

std::string LoadCommand::getDescription() {
    return "Loads a vEPROM FILE to be used for other commands";
}

std::string LoadCommand::getName() {
    return "load";
}

void LoadCommand::displayHelp(char* commandString) {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << commandString << " FILE" << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}
