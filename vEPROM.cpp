//============================================================================
// Name        : vEPROM.cpp
// Author      : tom
// Version     : 00
//===========================================================================
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <cstdio>
#include "vEPROM.h"

static int id = 0; // used to generate file for verprom
static std::string veprom_path = "vEPROM_"; // used to generate file for verprom

/**
 * @brief Set vEPROM to initial state (each byte to 0, etc.)
 */
int vEPROM::init() {
    std::ofstream fout(file_name, std::ios::binary);
    if (!fout) {
        std::string line = "Cannot open file for vEPROM\n";
        out_err(line);
        return -1;
    }
    fout.seekp(0, std::ios::beg);
    for (unsigned int i = 0; i < capacity; i++) {
        fout.put(0); 	// Initialize each byte to 0 in vEPROM
    }
    fout.close();
    files = {}; 		// Initialize files loaded/written
    bytes_loaded = 0; 	// Initialize number of bytes loaded
    last_valid_pos = 0;	// Initialize last valid position
    return 0;
}

/**
 * @brief Set bytes_loaded to a given size.
 *
 * @param size is the number of bytes to be loaded to vEPROM
 * @return 0: success; -1: fail if size > capacity.
 */
int vEPROM::set_bytes_loaded(unsigned int size) {
    if (size > capacity) { // size > capacity, no set and return -1
        return -1;
    } else { // size <= capacity, set bytes_loaded to size and return 0
        bytes_loaded = size;
        return 0;
    }
}

/**
 * @brief Construct vEPROM with a given capacity and set it to initial state
 *
 * @param cap is the given capacity in k bytes
 */
vEPROM::vEPROM(unsigned int cap) { // generate file name and create file for vEPROM
    capacity = cap;
    id++;
    file_name = veprom_path + std::to_string(id); // generate a file name for vEPROM
    init(); 	// Set vEPROM to initial state
}
#ifdef GUI_VEPROM
vEPROM::vEPROM(unsigned int cap, QTextEdit *guiOutText) {
    capacity = cap;
    id++;
    file_name = veprom_path + std::to_string(id); // generate a file name for vEPROM
    init(); 	// Set vEPROM to initial state
    guiOut = guiOutText;
}
#endif

/**
 * @brief Get file name for vEPROM
 *
 * @return: file name string for vEPROM
 */
std::string vEPROM::get_file_name() {
    return file_name;
}

/**
 * @brief Get file names with their sizes loaded/written to vEPROM
 *
 * @return: file names with their sizes in vector<pair<string, int>>
 */
std::vector<std::pair<std::string, int>> vEPROM::get_files() {
    return files;
}


/**
 * @brief Write a raw string to an address/position on vEPROM.
 *
 * @param position is the given address/position
 * @param s is the given string
 * @return 0: success; -1: fail to write if values to be written is out of vEPROM range, etc.
 */
int vEPROM::write_raw(unsigned int position, std::string s) {
    unsigned int n = s.size();
    std::string line;
    if (position + n > capacity) {
        line = "wrtie_raw out of vEPROM range\n";
        out_err(line);
        return -1;
    }
    std::fstream fout(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!fout) {
        line = "Cannot open file for vEPROM\n";
        out_err(line);
        return -1;
    }
    fout.seekp(position, std::ios::beg);
    for (unsigned int i = 0; i < n; i++) {
        fout.put(s[i]);
    }
    fout.close();
    line = "String written to vEPROM (" + file_name + ") at " + std::to_string(position) + "\n";
    out_normal(line);
    if (last_valid_pos < position + n) { // set the last valid position accordingly
        last_valid_pos = position + n;
    }
    return 0;
}


/**
 * @brief Show vEPROM's contents of n bytes from address on standard output.
 *  The contents shown are formated. Each line has a starting Address field,
 *  value of each of 16 bytes in hex and printable chars (. for char not printable).
 *   Address        |                 Values in Hex                 | Values in Char
 *   Hex   (Decimal)|0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |0123456789012345
 *   ---------------+-----------------------------------------------+----------------
 *        0(      0)|xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx|abcdefghijk=12AB
 *       10(     16)|xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx|..XYX9876...?*&)
 *
 * @param address is the given address/position
 * @param n is the number of bytes
 */
void vEPROM::format_out(unsigned int address, unsigned int n, std::ifstream &fin) {
    std::string line; 	// a line to be output
    char buffer[100];	// buffer for a line to be output
    unsigned int addr = address;	// Initialize starting addr
    unsigned int rem;
    unsigned int rb;	// remaining bytes in a line
    unsigned int i, j;
    unsigned int k;		// as index of buffer

    // Format title
    line = "Address        |                 Values in Hex                 | Values in Char\n";
    out_normal(line);
    line = "Hex   (Decimal)|0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |0123456789012345\n";
    out_normal(line);
    line = "---------------+-----------------------------------------------+----------------\n";
    out_normal(line);

    i = 0;
    while (i < n) {
        sprintf(buffer, "%6X(%7d)|", addr, addr);	// Format address in a line

        // Format a possible starting partial line
        k = 16;	// Start of values in hex
        rem = addr % 16;
        if (rem != 0) { // starting is a partial line
            for (j = 0; j < rem; j++) { // fill "   " and " " for bytes not in line
                buffer[k] = ' '; buffer[k+1] = ' ';
                buffer[k+2] = ' ';	// "   " for hex value of a byte
                buffer[j+64] = ' ';	// " " for a char
                k += 3;
            }
        }
        rb = 16 - rem;	// rb is number of remaining bytes in line to be output
        for (j = 0; j < rb && i < n; j++) {	// fill hex and char for each byte in line
            char c;
            fin.get(c);
            sprintf(&buffer[k], "%-2X", c);		// hex value of a byte
            buffer[k+2] = ' ';
            if (std::isprint(static_cast<unsigned char>(c)) ) { // c is printable
                buffer[j+64+rem] =  c;	// char of a byte
            } else {
                buffer[j+64+rem] = '.';
            }
            k += 3;
            addr++; i++;
        }
        buffer[63] = '|';
        sprintf(&buffer[80], "\n");	// add \n at end of buffer
        if (i < n) {	// middle full line or first line
            line = buffer;	// convert char array to string
            out_normal(line);
        }
    }

    // Format rest of a possible ending partial line
    rem = addr % 16;
    if (rem != 0) { // ending is a partial line
        rb = 16 - rem;	// rb is number of remaining bytes not in line
        for (j = 0; j < rb; j++) { // fill "   " and " " for bytes not in line
            sprintf(&buffer[k], "  "); 	// "   " for hex value of a byte
            buffer[k+2] = ' ';
            buffer[j+64+rem] = ' ';	// " " for a char
            k += 3;
        }
    }
    buffer[63] = '|';
    line = buffer;	// convert char array to string
    out_normal(line);
}


/**
 * @brief Read values at an address and length on EPROM and show them on standard output.
 *
 * @param address is the given address/position
 * @param n is the length
 * @return 0: success; -1: fail to read if values to be read is out of vEPROM range, etc.
 */
int vEPROM::read_raw(unsigned int address, unsigned int n) {
    std::string line; 	// a line to be output
    if (address + n > capacity) {
        line = "read_raw out of vEPROM range\n";
        out_err(line);
        return -1;
    }
    std::ifstream fin(file_name, std::ios::binary);
    if (!fin) {
        line = "Cannot open file for vEPROM\n";
        out_err(line);
        return -1;
    }
    fin.seekg(address, std::ios::beg);
    format_out(address, n, fin);
    fin.close();
    return 0;
}

/**
 * @brief Write/Append a file to the EPROM
 *
 * @param file is the name of the file
 * @return 0: success; -1: fail to write/append if file is too big, etc.
 */
int vEPROM::append_file(std::string file) { // append pair<file (name), size of file> to files
    std::string line; 	// a line to be output
    std::ifstream fin(file, std::ios::binary);
    if (!fin) {
        line = "Cannot open file: " + file + "\n";
        out_err(line);
        return -1;
    }

    fin.seekg(0, std::ios::end);
    unsigned int size = fin.tellg(); // get the size of file

    if (bytes_loaded + size > capacity) { // no enough space for file
        fin.close();
        line = "File is too big.\n";
        out_err(line);
        return -1;
    }

    // there is room for file
    std::fstream fout(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!fout) { // can not open file for EPROM
        fin.close();
        line = "Cannot open vEPROM file: " + file_name + "\n";
        out_err(line);
        return -1;
    }
    fout.seekp(bytes_loaded, std::ios::beg);
    fin.seekg(0, std::ios::beg);
    char c;
    while (fin.get(c)) {
        fout.put(c);
    }
    fin.close();
    fout.close();
    files.push_back({file, size});	// "append/concatenate" file in EPROM, and return 0
    line = file + "(" + std::to_string(size) + " bytes) written at " + std::to_string(bytes_loaded) + "\n";
    out_normal(line);
    bytes_loaded += size;

    if (last_valid_pos < bytes_loaded) last_valid_pos = bytes_loaded;
    return 0;
}

/**
 * @brief List the files with their sizes loaded/written to the EPROM
 */
void vEPROM::list() {
    std::string line; 	// a line to be output
    line = "Files written to EPROM (" + file_name + "): ";
    out_normal(line);
    for (auto x: files) {
        line = x.first + "(" + std::to_string(x.second) + ") ";
        out_normal(line);
    }
    line = "\n";
    out_normal(line);
}

/**
 * @brief Show each byte of file in vEPROM in standard output
 */
void vEPROM::read(std::string file) {
    std::string line; 	// a line to be output
    unsigned int start = 0; // start address of file in vEPROM
    unsigned int n = 0; // size of file
    unsigned int i;
    for (i = 0; i < files.size(); i++) { // find the given file written to vEPROM
        if (files[i].first == file) {
            n = files[i].second;
            break;
        } else {
            start += files[i].second;
        }
    } // n == 0: file not in vEPROM

    if (n == 0) { // file not in vEPROM
        line = "File not in vEPROM\n";
        out_err(line);
        return;
    }

    std::ifstream fin(file_name, std::ios::binary);
    if (!fin) {
        line = "Could not open vEPROM\n";
        out_err(line);
        return;
    }

    line = file + "(" + std::to_string(n) + " bytes)" + " starts at address " + std::to_string(start) + "\n";
    out_normal(line);

    fin.seekg(start, std::ios::beg);
    format_out(start, n, fin);
    fin.close();
}

/**
 * @brief Set the EPROM back to its original state
 */
void vEPROM::erase() {
    if (init() == 0) {
        std::string line; 	// a line to be output
        line = "Current EPROM is set to its original state\n";
        out_normal(line);
    }
}

