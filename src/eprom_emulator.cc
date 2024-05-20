/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 15:22:57 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-19 22:41:27
 */
#include "eprom_emulator.h"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#define MAX_FILE_IN_DATA_FOLDER 100
#define INI_FILE_NAME           "./data/eprom.ini"

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
    rom = new Eprom(path, size_in_kb * 1024);
    if (!rom->check_valid()) {
        return ERR_FAILED_TO_CREATE_OTHER;
    }
    save_path_to_ini(path);
    std::cout << "Successfully created " << path << std::endl;
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

    // Step 3: refresh mem and write to disk
    int ret;
    ret = rom->write_data(addr, s.length(), s.c_str());
    if (ret)
        return ret;

    std::cout << "Successfully wrote " << s << " to ADDRESS=" << addr << std::endl;

    return 0;
}

int EpromEmulator::read_raw(const int addr, const int len) {
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

    // Step 3: read data from mem
    char* p = (char*)malloc(len);
    int ret = rom->read_data(addr, len, p);
    if (ret)
        return ret;

    std::cout << "Successfully read " << len << " bytes from ADDRESS=" << addr
              << ". The content is:" << std::endl;
    std::cout << std::string(p, len) << std::endl;
    return 0;
}

int EpromEmulator::list() {
    // TODO: List available EPROM images
    return 0;
}

int EpromEmulator::read() {
    // TODO: Read EPROM contents
    return 0;
}

int EpromEmulator::write() {
    // TODO: Write EPROM contents
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

}  // namespace veprom
