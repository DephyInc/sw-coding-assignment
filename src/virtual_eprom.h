
#pragma once

#include <optional>

#include "simple_file_system.h"

/**
 * @class VirtualEprom
 * @brief Class to emulate the vEPROM
 *
 */
class VirtualEprom {
public:
    /**
     * @brief Create instance of a vEPROM that is stored in the specified file
     *
     */    
    VirtualEprom(std::string filename);

    /**
     * @brief Creates a new vEPROM with a specified capacity.
     *
     */    
    void create(int capacity);


    /**
     * @brief Writes a file to the vEPROM.
     *
     */    
    void writeFile(std::string filepath);


    /**
     * @brief Reads a file from the vEPROM.
     *
     */    
    std::string readFile(std::string filename);


    /**
     * @brief Writes a raw string of bytes to a specific address on the vEPROM.
     *
     */    
    void writeRaw(long address, std::string data);

    /**
     * @brief Reads the values stored at a specific address and length on 
     *        the vEPROM and returs it as a string.
     *
     */    
    std::string readRaw(long address, long length);


    /**
     * @brief Lists the file on the vEPROM to stdout.
     *
     */    
    void listFiles();


    /**
     * @brief Reset the EPROM back to its original state.
     *
     */    
    void erase();

private:
    std::string filename;
    

    /**
     * @brief Get capacity of the vEPROM.
     *
     */    
    long getCapacity();


    /**
     * @brief Calculate checksum of data buffer.
     *
     */    
    uint32_t calculateChecksum(char* data, long length);


    /**
     * @brief Save the file table to the vEPROM file.
     *
     */    
    void writeFileTable(FileTable& fileTable);


    /**
     * @brief Read the file table from the vEPROM file.
     *
     */    
    FileTable readFileTable();


    /**
     * @brief Read the file header at specified offset.
     *
     */    
    void readFileHeader(long offset, FileHeader& fileHeader);
};
