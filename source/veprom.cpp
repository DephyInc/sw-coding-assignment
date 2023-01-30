#include "veprom.h"
#include <stdio.h>

Veprom::eRetVal Veprom::create(size_t size, string & filename)
{
    // Choose a filename from "veprom_0.map" thru "veprom_255.map"
    FILE* fid = NULL;
    for (int i = 0; i < 256; i++)
    {
        char buf[32]; memset(buf, 0, sizeof(buf));
        sprintf(buf, "veprom_%i.map", i);
        fid = fopen(buf, "r");
        if (fid == NULL)
        {
            // Found free file - stop here
            filename = buf;
            fid = fopen(buf, "w");
            break;
        }
        fclose(fid);
    }

    // Ensure free file was found
    if (fid == NULL)
        return FilenamesExhausted;

    // Write binary buffer (zeros) to file
    uint8_t* data = (uint8_t*)malloc(size);
    if (data == NULL)
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