/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 13:03:23 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-19 15:47:41
 */

#pragma once
#include <cstdint>
#include <string>

#include "error_code.h"

namespace veprom {

class Eprom {
   private:
    class EpromImpl;  // encapsulated implementation
    EpromImpl* pImpl;

   public:
    Eprom(std::string& path);                  // load from path
    Eprom(std::string& path, const int size);  // create an empty new eprom
    ~Eprom();
    int check_valid();
    int erase();
    int read_data(const int target_addr, const int len, char* ptr);
    int write_data(const int target_addr, const int len, const char* ptr);
};

}  // namespace veprom