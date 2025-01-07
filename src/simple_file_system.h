
#pragma once
 
#define MAX_FILE_COUNT 16
#define MAX_FILENAME_LEN 64

struct FileTable {
    uint32_t checksum;
    uint32_t freeOffset;
    uint32_t fileOffsets[MAX_FILE_COUNT];  
};

struct FileHeader {
    uint32_t checksum;
    uint32_t size;
    char filename[MAX_FILENAME_LEN];
};
