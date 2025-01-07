
#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <memory>
#include <limits.h>
#include <unistd.h>

/**
 * @class Context
 * @brief Class to handle global information about the application environment
 *
 */
class Context {
private:
    std::string dataDirectory;

public:
    /**
     * @brief Gets the filename of the active vEprom file.
     *        This is the vEprom that’ll be used when commands are called.  
     */
    std::string readCurrentVepromFile() {
        std::string infileName = this->dataDirectory + "/.current";
        std::ifstream infile(infileName, std::ios::binary);
        if (infile) {
            infile.seekg(0, infile.end);
            int len = infile.tellg();
            auto buffer = std::make_unique<char[]>(len + 1);

            infile.seekg(0, infile.beg);
            infile.read(buffer.get(), len);
            infile.close();

            return buffer.get();
        }
        return nullptr;
    }

    /**
     * @brief Sets the filename of the active vEprom file.  
     *        This is the vEprom that’ll be used when commands are called.  
     *
     * @param currentVepromFile filename of the active vEprom
     */
    void saveCurrentVepromFile(std::string currentVepromFile) {
        std::string outfileName = this->dataDirectory + "/.current";
        std::ofstream outfile;
        outfile.open (outfileName, std::ios::out | std::ios::binary);
        outfile.write(currentVepromFile.c_str(), currentVepromFile.size());

    }

    /**
     * @brief Returns the path tho the directory where all files are save.
     */
    std::string getDataDirectory() {
        return this->dataDirectory;
    }

    /**
     * @brief Sets the path to the directory where all files are save.
     */
    void setDataDirectory(std::string dataDirectory) {
        // If data directory is absolute, then save it
        // Otherwise, get the current directory and append the data directory
        if (dataDirectory.rfind("/", 0) == 0) {
            this->dataDirectory = dataDirectory;
        } else {
            std::unique_ptr<char[]> dataPath(new char[PATH_MAX+1]);
            if (getcwd(dataPath.get(), sizeof(char)*PATH_MAX) != NULL) {
                strcat(dataPath.get(), "/");
                this->dataDirectory = dataPath.get() + dataDirectory;
            } else {
                this->dataDirectory = dataDirectory;
            }
        }
        this->dataDirectory.append("/");
    }
};
