/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 13:03:23 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-20 13:25:53
 */

#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "eprom_chip.h"
#include "error_code.h"

namespace veprom {

class EpromEmulator {
   public:
    EpromEmulator();
    ~EpromEmulator();

    int create(int size_in_kb);
    int load(std::string& path);
    int erase();
    int write_raw(const int addr, std::string& s);
    int read_raw(const int addr, const int len);
    int list();
    int read(std::string& filename);
    int write(std::string& path);

   private:
    // file index data structure
    struct FileIndex {
        int len;
        char name[PATH_LIMIT];
    };
    // data
    Eprom* rom;
    // interface
    int prepare_rom();
    std::string get_next_eprom_path();
    int save_path_to_ini(std::string& path);
    int check_ini_exist();
    std::string load_path_from_ini();
    std::string load_txt_file(std::string& path);
    std::string load_file_ptr(std::string filename);
    int get_next_file_addr();
    std::string extract_filename_from_path(std::string path);

    int load_file_index(int addr, struct FileIndex& fi);
};

}  // namespace veprom