/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 13:45:01 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-19 23:52:03
 */

#include "eprom_chip.h"

#include <sys/stat.h>

#include <cstring>
#include <fstream>
#include <iostream>

namespace veprom {

class Eprom::EpromImpl {
   private:
    int update_path(std::string& path);
    int save_to_disk();
    int load_from_disk();
    int size_check(const int addr);
    char* data_ptr;  // data pointer
    int data_size;   // size in byte
    std::string path;

   public:
    EpromImpl(std::string& path);                  // load an eprom from path
    EpromImpl(std::string& path, const int size);  // create an empty new eprom
    ~EpromImpl();

    int check_valid();
    int erase();

    int read_data(const int target_addr, const int len, char* ptr);
    int write_data(const int target_addr, const int len, const char* ptr);
};

Eprom::EpromImpl::EpromImpl(std::string& path) {
    update_path(path);
    load_from_disk();
}

Eprom::EpromImpl::EpromImpl(std::string& path, const int size) {
    if (FILE* file = fopen(path.c_str(), "r")) {
        fclose(file);
        std::cerr << "Failed to create file, file exists!" << std::endl;
        return;
    }
    update_path(path);
    data_size = size;
    data_ptr = (char*)malloc(data_size);
    if (!data_ptr) {
        std::cerr << "Malloc failed" << std::endl;
        return;
    }
    memset(data_ptr, 0, data_size);
    save_to_disk();
    return;
}

Eprom::EpromImpl::~EpromImpl() {
    if (data_ptr)
        free(data_ptr);
}

int Eprom::EpromImpl::check_valid() {
    return data_size != 0 && data_ptr != NULL;
}

int Eprom::EpromImpl::erase() {
    if (!data_ptr) {
        std::cerr << "Data not found" << std::endl;
        return ERR_DATA_NOT_FOUND;
    }
    // clear all data in mem
    memset(data_ptr, 0, data_size);

    // save mem cached data to disk(path)
    return save_to_disk();
}

int Eprom::EpromImpl::read_data(const int target_addr, const int len, char* ptr) {
    int ret;
    ret = size_check(target_addr + len - 1);  // the last address to be read
    if (ret)
        return ret;
    if (!ptr) {
        std::cerr << "PTR == NULL" << std::endl;
        return ERR_PTR_INVALID;
    }
    memcpy(ptr, data_ptr + target_addr, len);

    return 0;
}

int Eprom::EpromImpl::write_data(const int target_addr, const int len, const char* ptr) {
    int ret;
    ret = size_check(target_addr + len - 1);  // the last address to be written
    if (ret)
        return ret;
    if (!ptr) {
        std::cerr << "PTR == NULL" << std::endl;
        return ERR_PTR_INVALID;
    }
    memcpy(data_ptr + target_addr, ptr, len);

    // save mem cached data to disk(path)
    return save_to_disk();
}

int Eprom::EpromImpl::update_path(std::string& path_) {
    path = path_;
    return 0;
}

int Eprom::EpromImpl::load_from_disk() {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open file " << path << std::endl;
        return ERR_FILE_NOT_FOUND;
    }

    // Get the size of the file
    file.seekg(0, std::ios::end);
    int fsize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a buffer to hold the file data
    if (data_ptr) {
        free(data_ptr);  // avoid mem leak
    }
    data_ptr = (char*)malloc(fsize);
    if (!data_ptr) {
        std::cerr << "Malloc failed" << std::endl;
        return ERR_MALLOC;
    }

    // Read the file data into the buffer
    file.read(data_ptr, fsize);

    // Check if the file was read successfully
    if (!file) {
        std::cerr << "Failed to read file" << std::endl;
        return ERR_FAILED_TO_READ_FILE;
    }

    // Close the file
    file.close();
    data_size = fsize;
    return 0;
}

int Eprom::EpromImpl::save_to_disk() {
    std::ofstream outputFile(path, std::ios::binary);

    if (!outputFile) {
        std::cerr << "Failed to open file" << std::endl;
        return ERR_FILE_NOT_FOUND;
    }

    // Verify data
    if (!data_ptr) {
        std::cerr << "Data not found" << std::endl;
        return ERR_DATA_NOT_FOUND;
    }

    // Write the buffer data to the output file
    outputFile.write(data_ptr, data_size);

    // Check if the file was read successfully
    if (!outputFile) {
        std::cerr << "Failed to write to file" << std::endl;
        return ERR_FAILED_TO_WRITE_TO_FILE;
    }

    // Close the file
    outputFile.close();
    return 0;
}

int Eprom::EpromImpl::size_check(const int addr) {
    if (addr >= data_size) {
        std::cerr << "Read/write violation" << std::endl;
        return ERR_READ_WRITE_ADDRESS_VIOLATION;
    }
    return 0;
}

/*
* Declaration of Eprom
*/
Eprom::Eprom(std::string& path) : pImpl(new EpromImpl(path)) {
}
Eprom::Eprom(std::string& path, const int size) :
    pImpl(new EpromImpl(path, size)) {
}
Eprom::~Eprom() {
    delete pImpl;
}
int Eprom::check_valid() {
    return pImpl->check_valid();
}
int Eprom::erase() {
    return pImpl->erase();
}
int Eprom::read_data(const int target_addr, const int len, char* ptr) {
    return pImpl->read_data(target_addr, len, ptr);
}
int Eprom::write_data(const int target_addr, const int len, const char* ptr) {
    return pImpl->write_data(target_addr, len, ptr);
}

}  // namespace veprom
