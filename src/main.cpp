
#include <iostream>

#include "command_registry.h"
#include "create_command.h"
#include "load_command.h"
#include "list_command.h"
#include "erase_command.h"
#include "read_raw_command.h"
#include "write_raw_command.h"
#include "read_file_command.h"
#include "write_file_command.h"

#define DEFAULT_DATA_DIR ".veprom"

void displayHelp(CommandRegistry& registry) {
    
    std::cout << "Usage: veprom [OPTIONS] COMMAND" << std::endl;

    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  " << std::left << std::setw(11) << "-d PATH";
    std::cout << "Location of vEPROM data files (default \"./" << DEFAULT_DATA_DIR << "\")";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    registry.displayCommands();

    std::cout << "  " << std::left << std::setw(11) << "help";
    std::cout << "Display this help";
    std::cout << std::endl;

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {

    Context context;
    context.setDataDirectory(DEFAULT_DATA_DIR);

    // Setup command handlers
    CommandRegistry registry;
    registry.registerCommand<CreateCommand>();
    registry.registerCommand<LoadCommand>();
    registry.registerCommand<EraseCommand>();
    registry.registerCommand<ListCommand>();
    registry.registerCommand<ReadRawCommand>();
    registry.registerCommand<WriteRawCommand>();
    registry.registerCommand<ReadFileCommand>();
    registry.registerCommand<WriteFileCommand>();

    // Parse options
    int opt;
    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt)
        {
        case 'd':
            context.setDataDirectory(optarg);
            break;

        default:
            displayHelp(registry);
            return -1;
        }
    }

    // Handle commands
    if (argc-optind >= 1 && std::strcmp(argv[optind], "help") != 0) {

        auto cmd = registry.getCommand(argv[optind]);
        if (cmd != nullptr) {
            return cmd->run(context, argc-optind, &argv[optind]);
        } else {
            std::cout << "veprom: '" <<  argv[optind] << "' command is not supported.";
            std::cout << std::endl << std::endl;
        }
    }

    displayHelp(registry);
    return 0;

}
