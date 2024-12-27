
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

void displayHelp(CommandRegistry& registry) {
    
    std::cout << "Usage: veprom COMMAND" << std::endl;
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
    context.setDataDirectory("veprom-files");

    CommandRegistry registry;
    registry.registerCommand<CreateCommand>();
    registry.registerCommand<LoadCommand>();
    registry.registerCommand<EraseCommand>();
    registry.registerCommand<ListCommand>();
    registry.registerCommand<ReadRawCommand>();
    registry.registerCommand<WriteRawCommand>();
    registry.registerCommand<ReadFileCommand>();
    registry.registerCommand<WriteFileCommand>();

    if (argc >= 2 && std::strcmp(argv[1], "help") != 0) {
        auto cmd = registry.getCommand(argv[1]);
        if (cmd != nullptr) {
            return cmd->run(context, argc-1, &argv[1]);
        } else {
            std::cout << "veprom: '" << argv[1] << "' command is not supported.";
            std::cout << std::endl << std::endl;
        }
    }
    
    displayHelp(registry);
    return 0;

}
