// EPROM.h :
//
// This class processes command line arguments passed by the user.
// All the actual vEPROM code is in the VEPROM_CHIP class.

#include "vEPROM_Chip_Reader.h"

class EPROM {
public:
    int ProcessCMD(const std::string command, const std::string option1, const std::string option2);

    ~EPROM();

private:
    void SetOptions(const std::string option1, const std::string option2);

    void ReadConfig();

    void WriteConfig(std::string current_echip);

    void Create();

    void Load(bool silent = false);

    void WriteRaw();

    void ReadRaw();

    void Write();

    void List();

    void Read();

    void Erase();

    void Help();

    std::unordered_map<std::string, std::function<void()>> m_mapSwith =
    {
        {"create",      [&]() { Create(); }},
        {"load",        [&]() { Load(); }},
        {"write_raw",   [&]() { WriteRaw(); }},
        {"read_raw",    [&]() { ReadRaw(); }},
        {"write",       [&]() { Write(); }},
        {"list",        [&]() { List(); }},
        {"read",        [&]() { Read(); }},
        {"erase",       [&]() { Erase(); }},
    };
    std::string m_strOption1;
    std::string m_strOption2;
    VEPROM_CHIP* m_ptrCurrentChip = nullptr;
    const std::string m_strConfigFile = "eprom.config";
};
