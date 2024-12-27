
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <memory>

#include "virtual_eprom.h"
#include "simple_file_system.h"

VirtualEprom::VirtualEprom(std::string filename) {
    this->filename = filename;
}

void VirtualEprom::create(int capacity) {
    
    std::ofstream outfile(this->filename, std::ios::out | std::ios::binary);

    if (!outfile) {
        throw std::runtime_error("Failed to open vEPROM file");
    }
    
    // Initialize file table
    FileTable fileTable;
    std::memset((void*)&fileTable.fileOffsets, 0, MAX_FILE_COUNT * sizeof(uint32_t));
    fileTable.checksum = calculateChecksum((char*)&fileTable+sizeof(uint32_t), sizeof(FileTable)-sizeof(uint32_t));
    fileTable.freeOffset = sizeof(FileTable);

    // Initialize EPROM with 0xFF
    outfile.write((char*)&fileTable, sizeof(FileTable));
    for (int i = 0; i<capacity-sizeof(FileTable); i++) {
        outfile.put(0xFF);
    }
}

void VirtualEprom::writeFile(std::string filepath) {

    FileTable fileTable = readFileTable();

    // Find next empty slot in table for this file
    int index = -1;
    for (int i=0; i<MAX_FILE_COUNT; i++) {
        if (fileTable.fileOffsets[i] == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        throw std::runtime_error("Max file limit has been reached.  Cannot add file.");
    }

    // Open datafile
    std::ifstream infile(filepath, std::ios::binary);
    
    if (!infile) {
        throw std::runtime_error("Failed to open input file");
    }

    // Read data file
    infile.seekg(0, infile.end);
    int len = infile.tellg();
    auto buffer = std::make_unique<char[]>(len + 1);
    infile.seekg(0, infile.beg);
    infile.read(buffer.get(), len);
    infile.close();

    // Check if the file will fit
    int capacity = getCapacity();
    if (fileTable.freeOffset+sizeof(FileHeader)+len > capacity) {
        throw std::runtime_error("Data file will not fit on vEPROM");
    }
    
    // Open vEPROM file
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);

    if (!file) {
        throw std::runtime_error("Failed to open vEPROM file");
    }

    // Write file header
    FileHeader fileHeader;
    fileHeader.size = len;
    std::strncpy(fileHeader.filename, filepath.c_str(), MAX_FILENAME_LEN);
    fileHeader.checksum = calculateChecksum(buffer.get(), len);
    fileHeader.checksum ^= calculateChecksum((char*)&fileHeader-sizeof(uint32_t), sizeof(FileHeader)-sizeof(uint32_t));
    file.seekp(fileTable.freeOffset);
    file.write((char*)&fileHeader, sizeof(FileHeader));
    
    // Write file contents
    file.write(buffer.get(), len);
    file.close();

    // Write updated file table
    fileTable.fileOffsets[index] = fileTable.freeOffset;
    fileTable.freeOffset = fileTable.freeOffset + len + sizeof(FileHeader);
    writeFileTable(fileTable);
}

std::string VirtualEprom::readFile(std::string filename) {

    FileTable fileTable = readFileTable();
    FileHeader fileHeader;
    
    for (int i=0; i<MAX_FILE_COUNT; i++) {
        if (fileTable.fileOffsets[i] == 0) {
            break;
        }
        readFileHeader(fileTable.fileOffsets[i], fileHeader);
        if (strcmp(fileHeader.filename, filename.c_str()) == 0) {
            // TODO: validate checksum
            return readRaw(fileTable.fileOffsets[i]+sizeof(FileHeader), fileHeader.size);
        }
    }

    return "";
}

void VirtualEprom::writeRaw(long address, std::string data) {

    // Check if the data will fit
    int capacity = getCapacity();
    if (address+data.size() > capacity) {
        throw std::runtime_error("Data will not fit on vEPROM");
    }

    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    file.seekp(address);
    file.write(data.c_str(), data.size());
    file.close();
}

std::string VirtualEprom::readRaw(long address, long length) {

    // Check that we are not trying to read outside the vEPROM capacity
    int capacity = getCapacity();
    if (address+length > capacity) {
        throw std::runtime_error("Failed to read outside the vEPROM capacity");
    }
    
    auto buffer = std::make_unique<char[]>(length + 1);
    std::ifstream infile(this->filename, std::ios::binary);
    infile.seekg(address);
    infile.read(buffer.get(), length);
    infile.close();
    return buffer.get();
}

void VirtualEprom::listFiles() {
    
    FileTable fileTable = readFileTable();
    FileHeader fileHeader;
    
    for (int i=0; i<MAX_FILE_COUNT; i++) {
        if (fileTable.fileOffsets[i] == 0) {
            break;
        }
        readFileHeader(fileTable.fileOffsets[i], fileHeader);
        // TODO: validate checksum
        std::cout << fileHeader.filename << std::endl;
    }
}

void VirtualEprom::erase() {
    create(getCapacity());
}

long VirtualEprom::getCapacity() {
    std::ifstream infile(this->filename, std::ios::binary);
    if (infile) {
        infile.seekg(0, infile.end);
        return infile.tellg();
    }
    return -1;
}

uint32_t VirtualEprom::calculateChecksum(char* data, long length) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }
    return checksum;
}

void VirtualEprom::writeFileTable(FileTable& fileTable) {
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    fileTable.checksum = calculateChecksum((char*)&fileTable.freeOffset, sizeof(FileTable)-sizeof(uint32_t));
    file.write((char*)&fileTable, sizeof(FileTable));
    file.close();
}

FileTable VirtualEprom::readFileTable() {
    FileTable fileTable;
    std::ifstream file(this->filename, std::ios::binary);
    file.read((char*)&fileTable, sizeof(FileTable));
    file.close();
    return fileTable;
}

void VirtualEprom::readFileHeader(long offset, FileHeader& fileHeader){
    std::ifstream file(this->filename, std::ios::binary);
    file.seekg(offset);
    file.read((char*)&fileHeader, sizeof(FileHeader));
    file.close();
}
