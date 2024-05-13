// EPROM.cpp :
//

#include "EPROM.h"

int EPROM::ProcessCMD(const std::string command, const std::string option1, const std::string option2) {
    int ret = 0;

    // Read config to see if chip is already open
    ReadConfig();

    // Clear old options and set new ones.
    SetOptions(option1, option2);

    // This calls individual commands.
    try {
        const auto end = m_mapSwith.end();
        auto it = m_mapSwith.find(command);
        if (it != end) {
            it->second();
        }
        else {
            Help();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        ret = 1;
    }

    return ret;
}

EPROM::~EPROM() {
    if (m_ptrCurrentChip)
        delete m_ptrCurrentChip;
}

void EPROM::SetOptions(const std::string option1, const std::string option2) {
    m_strOption1 = option1;
    m_strOption2 = option2;
}

void EPROM::ReadConfig() {
    std::ifstream fi(m_strConfigFile);
    if (fi.good()) {
        std::string line;
        std::getline(fi, line);
        if (line.length() > 0) {
            m_strOption1 = line;
            Load(true);
        }
        fi.close();
    }
}

void EPROM::WriteConfig(std::string current_echip) {
    std::ofstream fo(m_strConfigFile);
    if (fo.good()) {
        fo << current_echip;
        fo.close();
    }
}

void EPROM::Create() {
    if (m_strOption1 == "")
        throw std::invalid_argument("Please enter size of the vEprom chip.");

    VEPROM_CHIP created(std::stoul(m_strOption1));
}

void EPROM::Load(bool silent) {
    if (m_strOption1 == "")
        throw std::invalid_argument("Please enter path of the vEprom file to load.");

    if (m_ptrCurrentChip)
        delete m_ptrCurrentChip;

    m_ptrCurrentChip = new VEPROM_CHIP(m_strOption1);

    if (!m_ptrCurrentChip->chipLoadError()) {
        WriteConfig(m_strOption1);

        if (!silent)
            std::cout << "Loaded epchip \"" << m_strOption1 << "\"." << std::endl;
    } else {
        if (!silent)
            std::cout << "Error loading epchip \"" << m_strOption1 << "\"." << std::endl;
    }
}

void EPROM::WriteRaw() {
    if (m_ptrCurrentChip == nullptr)
        throw std::invalid_argument("Please load vEprom chip before calling this operation.");
    if (m_strOption1 == "")
        throw std::invalid_argument("Please enter vEprom address to write.");
    if (m_strOption2 == "")
        throw std::invalid_argument("Please enter string to write to vEprom.");

    m_ptrCurrentChip->write_raw(std::stoul(m_strOption1), m_strOption2);
}

void EPROM::ReadRaw() {
    if (m_ptrCurrentChip == nullptr)
        throw std::invalid_argument("Please load vEprom chip before calling this operation.");
    if (m_strOption1 == "")
        throw std::invalid_argument("Please enter vEprom address to read.");
    if (m_strOption2 == "")
        throw std::invalid_argument("Please enter length of the vEprom read segment.");

    m_ptrCurrentChip->read_raw(std::stoul(m_strOption1), std::stoul(m_strOption2));
}

void EPROM::Write() {
    if (m_ptrCurrentChip == nullptr)
        throw std::invalid_argument("Please load vEprom chip before calling this operation.");
    if (m_strOption1 == "")
        throw std::invalid_argument("Please enter local path of file to write to the vEprom chip.");

    m_ptrCurrentChip->write(m_strOption1);
}

void EPROM::List() {
    if (m_ptrCurrentChip == nullptr)
        throw std::invalid_argument("Please load vEprom chip before calling this operation.");

    m_ptrCurrentChip->list();
}

void EPROM::Read() {
    if (m_ptrCurrentChip == nullptr)
        throw std::invalid_argument("Please load vEprom chip before calling this operation.");
    if (m_strOption1 == "")
        throw std::invalid_argument("Please enter path of the vEprom file to read.");

    m_ptrCurrentChip->read(m_strOption1);
}

void EPROM::Erase() {
    if (m_ptrCurrentChip == nullptr)
        throw std::invalid_argument("Please load vEprom chip before calling this operation.");

    m_ptrCurrentChip->erase();
}

void EPROM::Help() {
    std::cout << "vEPROM commands available: " << std::endl << std::endl;

    std::cout << "\t" << "create: creates a new virtual EPROM chip with a specified capacity.\n\t\tUsage: veprom create 256." << std::endl << std::endl;

    std::cout << "\t" << "load: loads a vEPROM file.\n\t\tUsage: veprom load /path/to/veprom/file." << std::endl << std::endl;

    std::cout << "\t" << "write_raw: writes a raw string of bytes to a specific address on the virtual EPROM chip.\n\t\tUsage: veprom write_raw $ADDRESS $STRING." << std::endl << std::endl;

    std::cout << "\t" << "read_raw: reads the values stored at a specific address and length on the virtual EPROM chip and prints it.\n\t\tUsage: veprom read_raw $ADDRESS $length." << std::endl << std::endl;

    std::cout << "\t" << "write: writes a file to the virtual EPROM chip.\n\t\tUsage: veprom write /path/to/local/file writes the file to the virtual EPROM chip as file." << std::endl << std::endl;

    std::cout << "\t" << "list: lists the files on the virtual EPROM chip.\n\t\tUsage: veprom list." << std::endl << std::endl;

    std::cout << "\t" << "read: reads a file from the virtual EPROM chip.\n\t\tUsage: veprom read $FILE reads the $FILE file from the virtual EPROM chip and prints it." << std::endl << std::endl;

    std::cout << "\t" << "erase: sets the EPROM back to its original state.\n\t\tUsage: veprom erase." << std::endl << std::endl;
}
