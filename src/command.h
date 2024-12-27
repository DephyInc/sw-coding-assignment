
#pragma once

#include <iostream>

#include "context.h"

/**
 * @class Command
 * @brief Base class for all commands
 *
 */
class Command {
public:
    /**
     * @brief Runs the command
     *
     * @param context Global information about the application environment
     * @param argc Number of arguments passed to the command
     * @param argv Pointer to the command line arguments for this command
     */
    virtual int run(Context& context, int argc, char* argv[]) = 0;

    /**
     * @brief Description of this command
     *
     */    
    virtual std::string getName() = 0;

    /**
     * @brief Name of this command
     *
     */    
    virtual std::string getDescription() = 0;
};
