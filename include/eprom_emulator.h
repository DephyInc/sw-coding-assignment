/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 13:03:23 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-19 16:43:47
 */

#pragma once
#include <cstdint>
#include <string>

#include "eprom_chip.h"
#include "error_code.h"

#define PATH_LIMIT 64

namespace veprom {

struct FileIndex {
    int addr;
    int len;
    char path[PATH_LIMIT];
};

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
    int read();
    int write();

   private:
    Eprom* rom;

    std::string get_next_eprom_path();
    int save_path_to_ini(std::string& path);
    int check_ini_exist();
    std::string load_path_from_ini();
};

}  // namespace veprom