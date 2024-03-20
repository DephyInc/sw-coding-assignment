#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cmath>

/// @brief Implementation of VEPROM By: Arash Agan 
/*
    * Video demonstration is uploaded to youtube https://youtu.be/nkI_sSfGi3E
    * Structure of virtual EPROM file

    - Number of files in the eprom (4 bytes)

    - Name of file1
    - Size of file1 (4 bytes)
    - Contents of file1

    - Name of file2
    - Size of file2 (4 bytes)
    - Contents of file2
*/
class fileHandler
{
private:
    std::string configFile{"dephy.cfg"};
    std::string filename{};
    int fileSizeB{};

    int getFilenameFromConfigFile();
    int create(int size, std::string &filePath);
    int load();
    int write_raw(int address, const std::string &String);
    int read_raw(int address, int length, std::string &String);
    int write(std::string &filePath);
    int list();
    int read(const std::string &filePath);
    int erase();
    void WriteStringToEndOfFile(const std::string &str);
    std::string ReadStringFromEndOfFile(const std::string &fname);

public:
    fileHandler(){};
    fileHandler(const std::string &filename, int fileSizeB)
        : filename(filename), fileSizeB(fileSizeB) {}
    int parseCommandlineArguments(int argc, char *argv[]);
};