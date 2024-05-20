/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 15:22:57 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-20 14:11:14
 */
#include "eprom_emulator.h"

#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

namespace veprom {

EpromEmulator::EpromEmulator() : rom(nullptr) {
}

EpromEmulator::~EpromEmulator() {
    if (rom) {
        delete rom;
    }
}

int EpromEmulator::create(int size_in_kb) {
    if (rom) {
        delete rom;
    }
    auto path = get_next_eprom_path();
    if (path.length() == 0) {
        return ERR_FAILED_TO_CREATE_FOLDER_FULL;
    }
    int cap = size_in_kb * 1024;
    rom = new Eprom(path, cap);
    if (!rom->check_valid()) {
        return ERR_FAILED_TO_CREATE_OTHER;
    }
    save_path_to_ini(path);
    std::cout << "Successfully created " << path << ". Capacity = " << cap
              << std::endl;
    return 0;
}

int EpromEmulator::load(std::string& path) {
    rom = new Eprom(path);
    if (!rom->check_valid()) {
        std::cout << "Failed to load  " << path << std::endl;
        return ERR_FAILED_TO_LOAD;
    }
    save_path_to_ini(path);
    std::cout << "Successfully loaded " << path << std::endl;
    return 0;
}

int EpromEmulator::erase() {
    // Step 1: load target EPROM from ini
    auto path = load_path_from_ini();
    if (path.length() == 0) {
        std::cout << "Please load an EPROM!" << std::endl;
        return ERR_INI_NOT_FOUND;
    }

    // Step 2: refresh EPROM from disk to mem
    rom = new Eprom(path);
    if (!rom->check_valid()) {
        std::cout << "Failed to load  " << path << std::endl;
        return ERR_FAILED_TO_LOAD;
    }

    // Step 3: erased and write to disk
    int ret;
    ret = rom->erase();
    if (ret)
        return ret;

    std::cout << "Successfully erased " << path << std::endl;
    return 0;
}

int EpromEmulator::write_raw(const int addr, std::string& s) {
    int ret;
    // Step 1: prepare rom
    ret = prepare_rom();
    if (ret)
        return ret;

    // Step 2: refresh mem and write to disk
    ret = rom->write_data(addr, s.length(), s.c_str());
    if (ret)
        return ret;

    std::cout << "Successfully wrote " << s << " to ADDRESS=" << addr << std::endl;

    return 0;
}

int EpromEmulator::read_raw(const int addr, const int len) {
    int ret;
    // Step 1: prepare rom
    ret = prepare_rom();
    if (ret)
        return ret;

    // Step 2: read data from mem
    char* p = (char*)malloc(len);
    ret = rom->read_data(addr, len, p);
    if (ret)
        return ret;

    std::cout << "Successfully read " << len << " bytes from ADDRESS=" << addr
              << ". The content is:" << std::endl;
    std::cout << std::string(p, len) << std::endl;
    return 0;
}

int EpromEmulator::list() {
    int ret;
    // Step 1: prepare rom
    ret = prepare_rom();
    if (ret)
        return ret;

    // check rom validity
    if (!rom || !rom->check_valid()) {
        std::cout << "Failed to load ROM!" << std::endl;
        return -1;
    }

    std::cout << "File list in ROM:" << std::endl;
    // iterate every file index in ROM
    int addr = 0;
    struct FileIndex fi;
    fi.len = 0;
    while (addr < rom->get_size()) {
        // printf("check addr=%d\n", addr);
        ret = load_file_index(addr, fi);
        if (fi.len == 0) {
            // std::cout << "New index ADDRESS = " << addr << std::endl;
            std::cout << std::endl;
            return 0;
        } else {
            printf("%10s: ADDRESS = %8d, SIZE = %8d\n", fi.name, addr, fi.len);
            // update addr to the end of current file
            addr += (int)sizeof(fi) + fi.len;
            // alignment
            addr = (addr + (~ALIGNMENT)) & ALIGNMENT;
        }
        // printf("update addr=%d\n", addr);
    }
    std::cout << std::endl;
    return 0;
}

int EpromEmulator::read(std::string& filename) {
    int ret;
    // Step 1: prepare rom
    ret = prepare_rom();
    if (ret)
        return ret;
    // check rom validity
    if (!rom || !rom->check_valid()) {
        std::cout << "Failed to load ROM!" << std::endl;
        return -1;
    }

    // Step 2: search filename
    int addr = 0;
    struct FileIndex fi;
    fi.len = 0;
    while (addr < rom->get_size()) {
        ret = load_file_index(addr, fi);
        if (fi.len == 0) {
            // std::cout << "New index ADDRESS = " << addr << std::endl;
            std::cout << "File NOT found in ROM!" << std::endl;
            return ERR_FILE_NOT_FOUND_IN_ROM;
        } else {
            // printf("%10s: ADDRESS = %8d, SIZE = %8d\n", fi.name, addr, fi.len);
            if (strcmp(fi.name, filename.c_str()) == 0) {
                // found the file
                char* txt_ptr = (char*)malloc(fi.len + 1);
                ret = rom->read_data(addr + sizeof(fi), fi.len, txt_ptr);
                txt_ptr[fi.len] = '\0';
                std::cout << txt_ptr;
                free(txt_ptr);
                return 0;
            }

            // update addr to the end of current file
            addr += (int)sizeof(fi) + fi.len;
            // alignment
            addr = (addr + (~ALIGNMENT)) & ALIGNMENT;
        }
    }
    return ERR_FILE_NOT_FOUND_IN_ROM;
}

int EpromEmulator::write(std::string& path) {
    int ret;
    // Step 1: prepare rom
    ret = prepare_rom();
    if (ret)
        return ret;

    // Step 2: prepare file
    // extract file name
    auto filename = extract_filename_from_path(path);

    // file name check
    if (filename.length() >= PATH_LIMIT) {
        std::cerr << "File name too long!" << std::endl;
        return ERR_FILE_NAME_TOO_LONG;
    }

    // load file content
    auto buffer = load_txt_file(path);

    // Gen file index
    struct FileIndex fi;
    fi.len = buffer.length();
    strcpy(fi.name, filename.c_str());

    // Step 3: start to write to file
    // get the next available space address
    int addr = get_next_file_addr();
    // capacity check
    if (addr < 0 || addr + (int)sizeof(fi) + fi.len > rom->get_size()) {
        std::cerr << "ROM capacity is not enough" << std::endl;
        return ERR_ROM_CAPACITY_NOT_ENOUGH;
    }

    // write file index
    ret = rom->write_data(addr, sizeof(fi), (char*)(&fi));
    if (ret)
        return ret;

    // write file content
    ret = rom->write_data(addr + sizeof(fi), fi.len, buffer.c_str());
    if (ret)
        return ret;

    std::cout << "Successfully write " << fi.name << " to ADDRESS=" << addr
              << std::endl;

    return 0;
}

int EpromEmulator::prepare_rom() {
    // Step 1: load target EPROM from ini
    auto path = load_path_from_ini();
    if (path.length() == 0) {
        std::cout << "Please load an EPROM!" << std::endl;
        return ERR_INI_NOT_FOUND;
    }

    // Step 2: refresh EPROM from disk to mem
    rom = new Eprom(path);
    if (!rom->check_valid()) {
        std::cout << "Failed to load  " << path << std::endl;
        return ERR_FAILED_TO_LOAD;
    }
    return 0;
}

std::string EpromEmulator::get_next_eprom_path() {
    // try to find the first unused ROMxx
    for (int i = 1; i < MAX_FILE_IN_DATA_FOLDER; ++i) {
        char ss[8];
        sprintf(ss, "%02d.bin", i);
        std::string path = "data/ROM" + std::string(ss);
        // std::cout << path << std::endl;
        if (FILE* file = fopen(path.c_str(), "r")) {
            fclose(file);
            continue;
        }
        return path;  // file not exists.
    }
    std::cerr << "./data/ folded is full!" << std::endl;
    return "";
}

int EpromEmulator::save_path_to_ini(std::string& path) {
    std::ofstream file(INI_FILE_NAME, std::ios::out);
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return ERR_INI_FAILED_TO_WRITE;
    }
    file << path << std::endl;  // Write the line to the file
    file.close();               // Close the file
    return 0;
}

int EpromEmulator::check_ini_exist() {
    std::string path = std::string(INI_FILE_NAME);
    return access(path.c_str(), 0) == 0;
}

std::string EpromEmulator::load_path_from_ini() {
    std::ifstream file(INI_FILE_NAME);  // Open the file for reading
    std::string line;
    if (!file) {
        std::cerr << "Failed to open ini file" << std::endl;
        return line;
    }
    std::getline(file, line);
    file.close();  // Close the file

    return line;
}

std::string EpromEmulator::load_txt_file(std::string& path) {
    // Create a buffer to hold the file data
    std::string buffer;

    // Open the file
    std::ifstream file(path, std::ios::in);
    if (!file) {
        std::cerr << "Failed to open file " << path << std::endl;
        return buffer;
    }
    // Get the size of the file
    file.seekg(0, std::ios::end);
    int fsize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file data into the buffer
    std::string line;
    while (std::getline(file, line)) {
        buffer += line + "\n";
    }
    // Close the file
    file.close();
    // std::cout << "File content:" << std::endl;
    // std::cout << buffer << std::endl;

    // Brief summary
    // std::cout << "File size =" << fsize << ". Loaded size =" << buffer.length()
    //           << std::endl;
    if (fsize != (int)buffer.length()) {
        std::cerr << "Warning: read size mismatch!" << std::endl;
    }
    std::cout << "Loaded file size = " << buffer.length() << std::endl;
    return buffer;
}

std::string EpromEmulator::extract_filename_from_path(std::string path) {
    std::string filename = "";
    for (auto i : path) {
        if (i == '/')
            filename = "";
        else
            filename += i;
    }
    return filename;
}

/*
* Get the next available address for new file
*/
int EpromEmulator::load_file_index(int addr, struct FileIndex& fi) {
    int ret;
    // check rom validity
    if (!rom || !rom->check_valid()) {
        std::cout << "Failed to load ROM!" << std::endl;
        return ERR_FAILED_TO_LOAD;
    }

    // check addr validity
    const int idx_sz = sizeof(struct FileIndex);
    if (addr >= rom->get_size() - idx_sz) {
    }

    // load fileindex
    char temp_buf[idx_sz];
    ret = rom->read_data(addr, idx_sz, temp_buf);
    if (ret)
        return ret;

    // save file index
    fi = *(struct FileIndex*)temp_buf;
    return 0;
}

/*
* Get the next available address for new file
* return value=address. -1: invalid address
*/
int EpromEmulator::get_next_file_addr() {
    // check rom validity
    if (!rom || !rom->check_valid()) {
        std::cout << "Failed to load ROM!" << std::endl;
        return -1;
    }

    // iterate to calculate next available addr
    int addr = 0;
    struct FileIndex fi;
    fi.len = 0;
    while (addr < rom->get_size()) {
        // printf("check addr=%d\n", addr);
        int ret = load_file_index(addr, fi);
        if (ret)
            return -1;
        if (fi.len == 0) {
            // std::cout << "New index ADDRESS = " << addr << std::endl;
            return addr;
        } else {
            // update addr to the end of current file
            addr += (int)sizeof(fi) + fi.len;
            // alignment
            addr = (addr + (~ALIGNMENT)) & ALIGNMENT;
        }
        // printf("update addr=%d\n", addr);
    }
    return -1;
}

}  // namespace veprom
