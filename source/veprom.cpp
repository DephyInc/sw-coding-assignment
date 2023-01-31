#include "veprom.h"
#include <stdio.h>
#include <iostream>

Veprom::eRetVal Veprom::create(size_t size, string & filename)
{
    // Choose a filename from "veprom_0.map" thru "veprom_255.map"
    FILE* fid = nullptr;
    for (int i = 0; i < N_VEPROMS_ALLOWED; i++)
    {
        char buf[SZ_FILENAME_BUF]; memset(buf, 0, sizeof(buf));
        sprintf(buf, "veprom_%i%s", i, FILENAME_EXT);
        fid = fopen(buf, "r");
        if (fid == nullptr)
        {
            // Found free file - stop here
            filename = buf;
            fid = fopen(buf, "w");
            break;
        }
        fclose(fid);
    }

    // Ensure free file was found
    if (fid == nullptr)
        return FilenamesExhausted;

    // Write binary buffer (zeros) to file
    uint8_t* data = (uint8_t*)malloc(size);
    if (data == nullptr)
        return MemoryAllocError;
    memset(data, 0, size);
    size_t retWrite = fwrite(data, sizeof(*data), size, fid);
    free(data);
    if (retWrite != size) 
        return FileCreationError;

    // Close file
    fclose(fid);
    return OK;
}

Veprom::eRetVal Veprom::load(string filename)
{
    // Make sure vEPROM file exists
    FILE* fidVeprom = fopen(filename.c_str(), "r");
    if (fidVeprom == nullptr)
        return ContextNotFound;
    fclose(fidVeprom);

    // Save context
    FILE* fidContext = fopen(FILENAME_CONTEXT, "w");
    if (fidVeprom == nullptr)
        return CannotOpenContext;
    size_t retWrite = fwrite(filename.c_str(), 1, filename.length(), fidContext);
    if (retWrite != filename.length())
        return CannotWriteContext;
    fclose(fidVeprom);

    return OK;
}

string Veprom::get_context()
{
    // Make sure vEPROM file exists
    FILE* fid = fopen(FILENAME_CONTEXT, "r");
    if (fid == nullptr)
    {
        // not found
        return "";
    }   
    char buf[SZ_FILENAME_BUF]; memset(buf, 0, sizeof(buf));
    fread(buf, 1, sizeof(buf) - 1, fid);
    string filename = buf;
    fclose(fid);
    return filename;
}

Veprom::eRetVal Veprom::write_raw(size_t addr, string data)
{
    // Get loaded context
    string filename = get_context();
    if (filename == "")
        return ContextNotLoaded;
    
    // Open file and get its size
    FILE* fid = fopen(filename.c_str(), "r");
    if (fid == NULL)
        return OpenFailedWriteRaw;
    fseek(fid, 0, SEEK_END);
    size_t size = ftell(fid);
    rewind(fid);

    // Check capacity
    if (addr + data.length() > size)
        return WriteOutOfBounds;

    // Read modify write
    uint8_t* buf = (uint8_t*)malloc(size);
    if (buf == 0)
        return MemoryAllocError;
    fread(buf, 1, size, fid);
    memcpy(buf + addr, data.c_str(), data.length());
    fclose(fid);
    fid = fopen(filename.c_str(), "w");
    fwrite(buf, 1, size, fid);
    fclose(fid);
    free(buf);
    return OK;
}

Veprom::eRetVal Veprom::read_raw(size_t addr, uint8_t* buf, size_t length)
{
    // Get loaded context
    string filename = get_context();
    if (filename == "")
        return ContextNotLoaded;

    // Check buffer
    if (buf == nullptr)
        return NullPtr;

    // Open file and get its size
    FILE* fid = fopen(filename.c_str(), "r");
    if (fid == NULL)
        return OpenFailedReadRaw;
    fseek(fid, 0, SEEK_END);
    size_t size = ftell(fid);
    rewind(fid);

    // Check capacity
    if (addr + length > size)
        return ReadOutOfBounds;

    // Read to buffer
    fseek(fid, addr, SEEK_SET);
    fread(buf, 1, length, fid);
    fclose(fid);
    return OK;
}