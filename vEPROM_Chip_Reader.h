// vEPROM_Chip_Reader.h :
//
// This class emulates an EPROM using flat files.
//
// For usage, see the HELP function.
//
// The following describes the flat file structure. Header and allocated space are at them beginning
// of the file. Dynamically growing list of file descriptors is at the end.

// HEADER
// -- size
// -- nFiles
// -- fileDataOffset
// ALLOCATED ADDRESS SPACE
// -- (specified in KBs by the user with the create operation)
// FILE DESCRIPTOR 1
// -- start_address
// -- length
// -- name
// FILE DESCRIPTOR 2
// ...

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>

// Header of the flat chip file.
typedef struct {
    unsigned long size;
    unsigned long nFiles;
    unsigned long fileDataOffset;
} VEPROM_CHIP_HEADER;

// File Descriptor structure.
typedef struct {
    unsigned long start_address;
    unsigned long length;
    char name[51];
} VEPROM_FD;

// To help with memory management.
class CStringBuddy {
public:
    CStringBuddy(unsigned long size) {
        m_cstr = new char[size + 1];
    }
    ~CStringBuddy() {
        delete[] m_cstr;
    }
    char* m_cstr;
};

class VEPROM_CHIP {
public:
    VEPROM_CHIP(unsigned long size);

    VEPROM_CHIP(const std::string name);

    void write_header();

    void write_file_descriptor(std::string file_name, unsigned long length, unsigned long offset);

    void write_raw(unsigned long address, std::string write_this);

    void read_raw(unsigned long address, unsigned long length);

    void write(std::string file_to_copy);

    void list();

    void read(std::string veprom_file_to_read);

    void erase();

    bool chipLoadError() { return (m_strName == "chip load error") ? true : false; };
private:
    VEPROM_CHIP_HEADER  m_structHeader = {};
    std::string m_strName;
};
