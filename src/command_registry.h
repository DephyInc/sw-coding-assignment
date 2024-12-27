
#pragma once

#include <iostream>
#include <map>
#include <iomanip>

#include "command.h"

/**
 * @class CommandRegistry
 */
class CommandRegistry {
private:
    std::map<std::string, std::unique_ptr<Command>> commands;
       
public:

    /**
     * @brief Register command of the specified type
     */
    template <typename T>
    void registerCommand() {
        auto cmd = std::make_unique<T>();
        commands.emplace(cmd->getName(), std::move(cmd));
    }
    
    /**
     * @brief Get pointer to the command that matches the name
     */
    Command* getCommand(std::string name) {
        try {
            return commands.at(name).get();
        } catch (const std::exception& e) {
            return nullptr;
        }
    }

    /**
     * @brief Display the names and descriptions of all the commands in the registry
     */
    void displayCommands() {
        for (auto& pair : commands) {
            std::cout << "  " << std::left << std::setw(11) << pair.first;
            std::cout << pair.second->getDescription();
            std::cout << std::endl;
        }
    }
};
