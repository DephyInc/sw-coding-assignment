/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 13:14:29 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-19 16:32:42
 */
#pragma once
namespace veprom {

enum veprom_error_code
{
    ERR_NONE,
    ERR_COMMAND,
    ERR_FAILED_TO_CREATE_FILE_EXISTS,
    ERR_FAILED_TO_CREATE_FOLDER_FULL,
    ERR_FAILED_TO_CREATE_OTHER,
    ERR_FAILED_TO_LOAD,
    ERR_MALLOC,
    ERR_FILE_NOT_FOUND,
    ERR_FAILED_TO_READ_FILE,
    ERR_FAILED_TO_WRITE_TO_FILE,
    ERR_DATA_NOT_FOUND,
    ERR_DATA_NOT_LOADED,
    ERR_READ_WRITE_ADDRESS_VIOLATION,
    ERR_PTR_INVALID,
    ERR_INI_NOT_FOUND,
    ERR_INI_FAILED_TO_READ,
    ERR_INI_FAILED_TO_WRITE,
};

static const int EPROM_SIZE_LIMIT = 1024;

}  // namespace veprom