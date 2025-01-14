#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cstring>
#include <algorithm>

namespace fs = std::filesystem;

struct FileEntry {
    char name[32];  // File name (max 31 chars + null terminator)
    size_t start;   // Start address in EPROM
    size_t size;    // Size of the file
};

class vEPROM {
private:
    std::string file_path;
    std::vector<uint8_t> data;
    std::vector<FileEntry> file_table;

    static constexpr size_t MAX_FILES = 128;
    static constexpr size_t HEADER_SIZE = MAX_FILES * sizeof(FileEntry);

    void validate_address(size_t address) {
        if (address >= data.size()) {
            throw std::out_of_range("Address out of range.");
        }
    }

    void update_file_table() {
        // Serialize the file table into the EPROM header
        std::memset(data.data(), 0xFF, HEADER_SIZE);
        std::memcpy(data.data(), file_table.data(), file_table.size() * sizeof(FileEntry));
    }

public:
    void create(size_t capacity) {
        if (capacity < HEADER_SIZE) {
            throw std::runtime_error("Capacity is too small for the file header.");
        }

        // Generate file name based on capacity
        std::ostringstream file_name;
        file_name << "veprom_" << (capacity / 1024) << ".bin";

        if (fs::exists(file_name.str())) {
            throw std::runtime_error("Virtual EPROM file '" + file_name.str() + "' already exists.");
        }

        data.resize(capacity, 0xFF); // EPROM is initialized to all 0xFF
        file_table.clear();
        update_file_table();
        file_path = file_name.str();

        // Save the virtual EPROM file
        std::ofstream file(file_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to create EPROM file.");
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());

        // Output the path of the created file
        std::cout << "Created virtual EPROM: " << fs::absolute(file_path) << std::endl;
        std::cout << "Data size: " << data.size() << " bytes\n";
    }

    void load(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file.");
        }

        // Determine file size
        file.seekg(0, std::ios::end);
        std::streamsize file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (file_size < HEADER_SIZE) {
            throw std::runtime_error("File size is too small to be a valid EPROM.");
        }

        // Resize data and read file contents
        data.resize(file_size, 0xFF);

        file.read(reinterpret_cast<char*>(data.data()), file_size);
        if (!file) {
            throw std::runtime_error("Failed to read EPROM file contents.");
        }

        file_path = path;

        // Deserialize the file table from the EPROM header
        file_table.clear();
        for (size_t i = 0; i < MAX_FILES; ++i) {
            FileEntry entry;
            std::memcpy(&entry, data.data() + i * sizeof(FileEntry), sizeof(FileEntry));
            if (entry.name[0] != '\0' && entry.name[0] != (char)0xFF) {
                file_table.push_back(entry);
            }
        }

        std::cout << "Successfully loaded virtual EPROM: \"" << fs::absolute(file_path) << "\"\n";
        std::cout << "Data size: " << data.size() << " bytes\n";
    }

    void write_file(const std::string& local_file) {
        std::ifstream file(local_file, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open local file.");
        }

        std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (buffer.size() > data.size() - HEADER_SIZE) {
            throw std::runtime_error("File size exceeds EPROM capacity.");
        }

        size_t start_address = HEADER_SIZE;
        for (const auto& entry : file_table) {
            start_address = std::max(start_address, entry.start + entry.size);
        }

        if (start_address + buffer.size() > data.size()) {
            throw std::runtime_error("Not enough space in EPROM for the file.");
        }

        // Add file to table
        FileEntry entry = {};
        std::strncpy(entry.name, fs::path(local_file).filename().string().c_str(), sizeof(entry.name) - 1);
        entry.start = start_address;
        entry.size = buffer.size();
        file_table.push_back(entry);
        update_file_table();

        // Write file data
        std::copy(buffer.begin(), buffer.end(), data.begin() + entry.start);
    }

    void read_file(const std::string& file_name) {
        auto it = std::find_if(file_table.begin(), file_table.end(), [&](const FileEntry& entry) {
            return file_name == entry.name;
        });

        if (it == file_table.end()) {
            throw std::runtime_error("File not found on EPROM.");
        }

        // Output file content to stdout
        std::cout.write(reinterpret_cast<const char*>(data.data() + it->start), it->size);
        std::cout << std::endl;
    }

    void write_raw(size_t address, const std::string& content) {
        validate_address(address);
        if (address + content.size() > data.size()) {
            throw std::out_of_range("Content exceeds EPROM boundary.");
        }
        for (size_t i = 0; i < content.size(); ++i) {
            data[address + i] = static_cast<uint8_t>(content[i]);
        }
    }

    void read_raw(size_t address, size_t length) {    
        if (data.empty()) {
            throw std::runtime_error("EPROM data is empty.");
        }

        if (address >= data.size()) {
            throw std::out_of_range("Address out of range.");
        }
        if (address + length > data.size()) {
            throw std::out_of_range("Length exceeds EPROM boundary.");
        }
        
        std::cout << "Data at requested range:\n";
        for (size_t i = 0; i < length; ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[address + i] << " ";
        }
        std::cout << std::dec << "\n";
    }

    void list_files() {
        if (file_table.empty()) {
            std::cout << "No files stored on EPROM." << std::endl;
            return;
        }

        std::cout << "Files on virtual EPROM:\n";
        for (const auto& entry : file_table) {
            std::cout << "  Name: " << entry.name << ", Size: " << entry.size << " bytes\n";
        }
    }

    void erase() {
        std::fill(data.begin(), data.end(), 0xFF);
        file_table.clear();
        update_file_table();
    }

    void save_state() {
        std::ofstream state_file("veprom_state.bin", std::ios::binary);
        if (!state_file) {
            throw std::runtime_error("Failed to save state.");
        }

        size_t data_size = data.size();
        state_file.write(reinterpret_cast<const char*>(&data_size), sizeof(data_size));
        state_file.write(reinterpret_cast<const char*>(data.data()), data_size);

        size_t file_table_size = file_table.size();
        state_file.write(reinterpret_cast<const char*>(&file_table_size), sizeof(file_table_size));
        state_file.write(reinterpret_cast<const char*>(file_table.data()), file_table_size * sizeof(FileEntry));
    }

    void load_state() {
        std::ifstream state_file("veprom_state.bin", std::ios::binary);
        if (!state_file) {
            return; // No state to load
        }

        size_t data_size = 0;
        state_file.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));
        data.resize(data_size);
        state_file.read(reinterpret_cast<char*>(data.data()), data_size);

        size_t file_table_size = 0;
        state_file.read(reinterpret_cast<char*>(&file_table_size), sizeof(file_table_size));
        file_table.resize(file_table_size);
        state_file.read(reinterpret_cast<char*>(file_table.data()), file_table_size * sizeof(FileEntry));
    }

};

void print_usage() {
    std::cout << "Usage:\n"
              << "  veprom create <capacity_in_kb>\n"
              << "  veprom load  /path/to/veprom/file\n"
              << "  veprom write /path/to/local/file\n"
              << "  veprom read  <file_name>\n"
              << "  veprom write_raw <address> <string>\n"
              << "  veprom read_raw  <address> <length>\n"
              << "  veprom list\n"
              << "  veprom erase\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    vEPROM veprom;

    try {
        veprom.load_state(); // Load previous state

        std::string command = argv[1];

        if (command == "create" && argc == 3) {
            size_t capacity_in_kb = std::stoul(argv[2]);
            veprom.create(capacity_in_kb * 1024); // Convert KB to bytes
        } else if (command == "load" && argc == 3) {
            veprom.load(argv[2]);
        } else if (command == "write" && argc == 3) {
            veprom.write_file(argv[2]);
        } else if (command == "read" && argc == 3) {
            veprom.read_file(argv[2]);
        } else if (command == "write_raw" && argc == 4) {
            size_t address = std::stoul(argv[2]);
            veprom.write_raw(address, argv[3]);
        } else if (command == "read_raw" && argc == 4) {
            size_t address = std::stoul(argv[2]);
            size_t length = std::stoul(argv[3]);
            veprom.read_raw(address, length);
        } else if (command == "list") {
            veprom.list_files();
        } else if (command == "erase") {
            veprom.erase();
        } else {
            print_usage();
            return 1;
        }

        veprom.save_state(); // Save state after each operation
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
