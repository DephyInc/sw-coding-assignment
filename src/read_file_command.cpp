
#include "read_file_command.h"
#include "virtual_eprom.h"

int ReadFileCommand::run(Context& context, int argc, char* argv[]) {

    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        displayHelp(argv[0]);
        return 0;
    }
    
    auto filename = context.readCurrentVepromFile();
    std::string dataFile = argv[1];
    
    VirtualEprom eprom{filename};
    auto data = eprom.readFile(dataFile);
    
    std::cout << data << std::endl;
    
    return 0;
}

std::string ReadFileCommand::getDescription() {
    return "Reads a FILE from the vEPROM chip and outputs it to stdout";
}

std::string ReadFileCommand::getName() {
    return "read";
}

void ReadFileCommand::displayHelp(char* commandString) {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << commandString << " FILE" << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}
