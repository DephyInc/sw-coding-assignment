
#include <iostream>
#include <time.h>

#include "create_command.h"
#include "virtual_eprom.h"

int CreateCommand::run(Context& context, int argc, char* argv[]) {

    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        displayHelp(argv[0]);
        return 0;
    }
    
    // Convert argument string to long
    long capacity;
    try {
        capacity = std::stol(argv[1]) * 1024;
    } catch (const std::invalid_argument& e) {
        capacity = -1;
    } catch (const std::out_of_range& e) {
        capacity = -1;
    }

    // Display help & return error if capacity value is invalid
    if (capacity <= 0) {
        std::cout << "Invalid value for capacity" << std::endl;
        displayHelp(argv[0]);
        return -1;
    }
    
    auto filename = context.getDataDirectory() + generateFilename(20);
    
    VirtualEprom eprom{filename};
    eprom.create(capacity);
    
    context.saveCurrentVepromFile(filename);
    
    std::cout << "New vEPROM created as file " << filename << std::endl;
    
    return 0;
}

std::string CreateCommand::generateFilename(int len) {

    const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuv";
    int stringLength = sizeof(alphanum) - 1;
    std::string filename;
    
    srand(time(NULL)); 
    for (int i = 0; i < len-4; i++) {
        filename += alphanum[rand() % stringLength];
    }
    filename.append(".bin");
    
    return filename;
}

std::string CreateCommand::getDescription() {
    return "Creates a new virtual EPROM chip with a specified CAPACITY";
}

std::string CreateCommand::getName() {
    return "create";
}

void CreateCommand::displayHelp(char* commandString) {
    std::cout << std::endl;        
    std::cout << "Usage: veprom " << commandString << " CAPACITY" << std::endl;
    std::cout << std::endl;
    std::cout << getDescription() << std::endl;
}


