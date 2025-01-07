
#pragma once

#include "command.h"

/**
 * @class ReadRawCommand
 * @brief Class to handle the read_raw command 
 *
 */
class ReadRawCommand : public Command {
public:
    /**
     * @brief Runs the read_raw command.
     *        Reads the values stored at a specific address and length 
     *        on the virtual EPROM chip and outputs it on stdout.
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
