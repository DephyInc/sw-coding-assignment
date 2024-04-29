#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

struct FileRecord {
    size_t address_;
    string fname_;
    size_t data_;
    size_t len_;
};

class VEprom {
public:
    VEprom();
    ~VEprom();
    void Verbose();
    int Create (int size);
    int	Load (const char* eprom_name);
    int WriteRaw (int address, const char* str);
    int ReadRaw (int address, int len);
    int Write (const char* fname);
    int List (bool print, bool updateHTML);
    int Read (const char* fname);
    int Erase();
protected:
    int RestoreMem();
    int SaveMem();
    int FindAvailable();
    size_t FindFile(const char* fname);
    int WriteHTML();
private:
    string eprom_name_;
    bool v_;
    vector<unsigned char> mem_;
    size_t memtop_;
    size_t memend_;
    bool dirty_;
    vector<FileRecord> files_;
};

