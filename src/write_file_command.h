
#pragma once

#include "command.h"

/**
 * @class WriteFileCommand
 * @brief Class to handle the write file command
 *
 */
class WriteFileCommand : public Command {
public:
    /**
     * @brief Runs the write file command
     *
     * @param context Global information about the application environment
     * @param argc Number of arguments passed to the command
     * @param argv Pointer to the command line arguments for this command
     */
    int run(Context& context, int argc, char* argv[]) override;

    /**
     * @brief Description of this command
     *
     */    
    std::string getDescription() override;

    /**
     * @brief Name of this command
     *
     */    
    std::string getName() override;
    
private:
    /**
     * @brief Display the help for this command
     *
     */
    void displayHelp();
};
