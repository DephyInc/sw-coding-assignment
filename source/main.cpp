#include <iostream>
#include "veprom.h"

#define __validate(condition, ret) \
    if (!(condition)) \
    {cout << Veprom::err_descr(ret); return ret;}

int main(int argc, char* argv[])
{
    __validate(argc > 1, Veprom::MissingArgs);

    // Create vEPROM instance
    Veprom veprom = Veprom();
    Veprom::eRetVal ret = Veprom::__Unknown__;

    // Method: "create"
    if (strcmp(argv[1], METHOD_CREATE) == 0)
    {
        // Get vEPROM size (limit to 1024 KB)
        __validate(argc > 2, Veprom::MissingArgs);
        int szKB = atoi(argv[2]);
        __validate(szKB >= 0, Veprom::InvalidArgs);
        __validate(szKB <= 1024, Veprom::NotSupported);

        // Create vEPROM and output filename
        string filename;
        ret = veprom.create(szKB * 1024, filename);
        __validate(ret == Veprom::OK, ret);
        cout << filename;
        return 0;
    }
    else if (strcmp(argv[1], METHOD_LOAD) == 0)
    {
        // Get choice of file to load
        __validate(argc > 2, Veprom::MissingArgs);
        string filename = argv[2];
        
        // Setup choice
        ret = veprom.load(filename);
        __validate(ret == Veprom::OK, ret);
        return 0;
    }
    else if (strcmp(argv[1], METHOD_WRITE_RAW) == 0)
    {
        // Get address and payload
        __validate(argc > 2, Veprom::MissingArgs);
        // -----------------------------------------------
        // TODO: differentiate addr = 0 and addr = invalid
        // -----------------------------------------------
        int addr = atoi(argv[2]); 
        __validate(argc > 3, Veprom::MissingArgs);
        string data = argv[3];

        // Execute write raw
        ret = veprom.write_raw(addr, (uint8_t*)data.c_str(), data.length());
        __validate(ret == Veprom::OK, ret);
        return 0;
    }
    else if (strcmp(argv[1], METHOD_READ_RAW) == 0)
    {
        // Get address and length
        __validate(argc > 2, Veprom::MissingArgs);
        // -----------------------------------------------
        // TODO: differentiate addr = 0 and addr = invalid
        // -----------------------------------------------
        int addr = atoi(argv[2]); 
        __validate(argc > 3, Veprom::MissingArgs);
        int length = atoi(argv[3]);
        __validate(length > 0, Veprom::InvalidArgs);

        // Execute read raw
        uint8_t* buf = (uint8_t*)malloc(length); memset(buf, 0, length);
        __validate(buf != nullptr, Veprom::MemoryAllocError);
        ret = veprom.read_raw(addr, buf, length);
        __validate(ret == Veprom::OK, ret);
        for (int i = 0; i < length; i++)
            cout << buf[i];
        free(buf);
        return 0;
    }
    else if (strcmp(argv[1], METHOD_WRITE) == 0)
    {
        // Get address and length
        __validate(argc > 2, Veprom::MissingArgs);
        string filename = argv[2];

        // Find file
        FILE* fid = fopen(filename.c_str(), "r");
        __validate(fid != nullptr, Veprom::NotFound);

        // Get file size
        fseek(fid, 0, SEEK_END);
        size_t size = ftell(fid);
        rewind(fid);
        
        // Get buffer
        uint8_t* buf = (uint8_t*)malloc(size); memset(buf, 0, size);
        __validate(buf != nullptr, Veprom::MemoryAllocError); 
        fread(buf, 1, size, fid);
        ret = veprom.write(filename, buf, size);
        __validate(ret == Veprom::OK, ret);
        free(buf);
        fclose(fid);
        return 0;
    }
    else if (strcmp(argv[1], METHOD_LIST) == 0)
    {
        // Get list of files
        vector<string> files = {};
        ret = veprom.list(files);
        __validate(ret == Veprom::OK, ret);
        for (int i = 0; i < files.size(); i++)
            cout << files[i] << '\n';
        return 0;
    }
    else if (strcmp(argv[1], METHOD_READ) == 0)
    {
        // Read a file
        __validate(argc > 2, Veprom::MissingArgs);
        string filename = argv[2];

        // Request file
        uint8_t* buf = nullptr;
        size_t len = 0;
        ret = veprom.read(filename, &buf, &len);
        if (ret != Veprom::OK)
        { 
            // Read file error
            free(buf);
            __validate(false, ret);
        }
        // print contents
        for (int i = 0; i < len; i++)
            cout << buf[i];
        return 0;
    }
    else if (strcmp(argv[1], METHOD_ERASE) == 0)
    {
        // erase disk
        ret = veprom.erase();
        __validate(ret == Veprom::OK, ret);
        return 0;
    }
    else
    {   // Method not found
        __validate(false, Veprom::InvalidMethod);
    }
}