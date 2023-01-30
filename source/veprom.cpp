#include "veprom.h"
#include <stdio.h>

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
    if (fidVeprom == NULL)
        return ContextNotFound;
    fclose(fidVeprom);

    // Save context
    FILE* fidContext = fopen(FILENAME_CONTEXT, "w");
    if (fidVeprom == NULL)
        return CannotOpenContext;
    size_t retWrite = fwrite(filename.c_str(), 1, filename.length(), fidContext);
    if (retWrite != filename.length())
        return CannotWriteContext;
    fclose(fidVeprom);

    return OK;
}