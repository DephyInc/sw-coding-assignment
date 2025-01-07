
#pragma once

struct FileInfo {
    std::string filename;
    uint32_t offset;
    uint32_t size;
    bool valid;  // Check sum matches
};
