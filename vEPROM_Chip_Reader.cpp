// vEPROM_Chip_Reader..cpp :
//

#include <cstring>

#include "vEPROM_Chip_Reader.h"

VEPROM_CHIP::VEPROM_CHIP(unsigned long size) {

    // Find unused name.
    int ind = 1;
    char tempstr[50];
    while (1) {
        strcpy(tempstr, (std::to_string(ind) + ".epchip").c_str());
        std::ifstream fi(tempstr);
        if (!fi.good())
            break;
        ind++;
    }
    m_strName = tempstr;

    // Save the chip.
    m_structHeader.size = size * 1024;
    std::ofstream fo(m_strName);
    if (fo.good()) {
        fo.write((const char*)&m_structHeader, sizeof(VEPROM_CHIP_HEADER));
        fo.write(std::string(m_structHeader.size, ' ').c_str(), m_structHeader.size);
        fo.close();
    }
    else
        throw std::runtime_error("Error: could not create epchip.");

    // Print chip name.
    std::cout << "Created epchip \"" << m_strName << "\"." << std::endl;
}

VEPROM_CHIP::VEPROM_CHIP(const std::string name)
{
    m_strName = name;

    // Read the epfrom chip.
    std::ifstream fi(m_strName);
    if (fi.good()) {
        fi.read((char*)&m_structHeader, sizeof(VEPROM_CHIP_HEADER));
        fi.close();
    }
    else
        m_strName = "chip load error";
}

void VEPROM_CHIP::write_header() {
    std::fstream fo(m_strName);
    if (fo.good()) {
        fo.write((const char*)&m_structHeader, sizeof(VEPROM_CHIP_HEADER));
        fo.close();
    }
    else
        throw std::runtime_error("Epchip file write error.");
}

void VEPROM_CHIP::write_file_descriptor(std::string file_name, unsigned long length, unsigned long offset) {
    std::fstream fo(m_strName, std::ios::app);
    if (fo.good()) {
        VEPROM_FD fd;
        std::string fname = file_name;
        size_t found = file_name.find_last_of("/\\");
        if (found != std::string::npos)
           fname = file_name.substr(found+1);
        size_t flen = std::min(sizeof(fd.name) - 1, fname.length());
        memset(fd.name, 0, sizeof(fd.name));
        memcpy(fd.name, fname.c_str(), flen);
        fd.length = length;
        fd.start_address = offset;
        fo.write((const char*)&fd, sizeof(VEPROM_FD));
        fo.close();
    }
    else
        throw std::runtime_error("Error: epchip file write error.");
}

void VEPROM_CHIP::write_raw(unsigned long address, std::string write_this) {
    if (address + write_this.length() > m_structHeader.size)
        throw std::invalid_argument("Error: request out of epchip size range.");

    std::fstream fo(m_strName);
    if (fo.good()) {
        fo.seekp(address + sizeof(VEPROM_CHIP_HEADER), std::ios::beg);
        fo << write_this;
        fo.close();
    }
    else
        throw std::runtime_error("Error: epchip file write error.");
}

void VEPROM_CHIP::read_raw(unsigned long address, unsigned long length) {
    if (address + length > m_structHeader.size)
        throw std::invalid_argument("Error: request out of epchip size range.");

    std::ifstream fi(m_strName);
    if (fi.good()) {
        CStringBuddy cb(length);
        fi.seekg(address + sizeof(VEPROM_CHIP_HEADER));
        fi.read(cb.m_cstr, length);
        cb.m_cstr[length] = '\0';
        fi.close();
        std::cout << cb.m_cstr << std::endl;
    }
    else
        throw std::runtime_error("Error: epchip file read error.");
}

void VEPROM_CHIP::write(std::string file_to_copy) {
    std::ifstream fi(file_to_copy);
    if (fi.good()) {
        std::ostringstream sstr;
        sstr << fi.rdbuf();
        std::string strtemp = sstr.str();
        write_raw(m_structHeader.fileDataOffset, strtemp);
        fi.close();
        write_file_descriptor(file_to_copy, (unsigned long)strtemp.length(), m_structHeader.fileDataOffset);
        m_structHeader.fileDataOffset += (unsigned long)strtemp.length();
        m_structHeader.nFiles++;
        write_header();
        std::cout << "Wrote file \"" << file_to_copy << "\" to epchip." << std::endl;
    }
    else
        throw std::runtime_error("Local file not found on disk.");
}

void VEPROM_CHIP::list() {
    std::ifstream fi(m_strName);
    if (fi.good()) {
        VEPROM_FD fd;
        fi.seekg(sizeof(VEPROM_CHIP_HEADER) + m_structHeader.size);
        while (1) {
            fi.read((char*)&fd, sizeof(VEPROM_FD));
            if (fi.gcount() > 0) {
                std::cout << fd.name << std::endl;
            }
            else
                break;
        }
        fi.close();
    }
    else
        throw std::runtime_error("Could not get a listing of files on epchip.");
}

void VEPROM_CHIP::read(std::string veprom_file_to_read) {
    std::ifstream fi(m_strName);
    if (fi.good()) {
        VEPROM_FD fd;
        bool found = false;
        fi.seekg(sizeof(VEPROM_CHIP_HEADER) + m_structHeader.size);
        while (1) {
            fi.read((char*)&fd, sizeof(VEPROM_FD));
            if (fi.gcount() > 0) {
                if (veprom_file_to_read == fd.name) {
                    std::cout << fd.name << std::endl;
                    read_raw(fd.start_address, fd.length);
                    found = true;
                }
            }
            else
                break;
        }
        fi.close();
        if (!found)
            throw std::invalid_argument("File not found on epchip.");
    }
    else
        throw std::runtime_error("Could not read file from epchip.");
}

void VEPROM_CHIP::erase() {
    m_structHeader.fileDataOffset = m_structHeader.nFiles = 0;

    std::ofstream fo(m_strName);
    if (fo.good()) {
        fo.write((const char*)&m_structHeader, sizeof(VEPROM_CHIP_HEADER));
        fo.write(std::string(m_structHeader.size, ' ').c_str(), m_structHeader.size);
        fo.close();
    }
    else
        throw std::runtime_error("Error: could not erase epchip.");
}
