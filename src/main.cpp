#include "appwindow.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include <limits>
#include <filesystem>
#include <sys/stat.h>

#include <string_view>
#include <algorithm>

struct FileDescriptor
{
    char fname[50]; // Assuming a maximum filename length of 50 characters
    size_t start;
    size_t length;
};

/**
 * @brief A class representing a virtual EPROM (vEPROM) emulator that allows users to simulate
 *  reading, writing, listing, and erasing files stored in a virtual EPROM chip.
 *
 * Member Variables:
 *      filename: A string representing the filename of the vEPROM file.
 *      capacity: An unsigned integer representing the total capacity of the vEPROM chip in bytes.
 */
class VirtualEPROM
{
private:
    std::string filename;
    size_t capacity;

    /**
     * @brief Writes raw data to a specific address in the virtual EPROM chip.
     *
     * This method allows writing raw data to a specified address within the virtual EPROM chip.
     *
     * @param address The starting address in the chip where the data will be written.
     * @param data    The data to be written to the chip.
     */
    void write_raw(size_t address, const std::string &data)
    {
        std::ofstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
        if (!file)
        {
            std::cerr << "Error: Failed to open vEPROM file: " << filename << std::endl;
            return;
        }

        file.seekp(address);
        file.write(data.c_str(), data.size());

        file.close();
    }

    /**
     * @brief Reads raw data from a specific address in the virtual EPROM chip.
     *
     * This method allows reading raw data from a specified address within the virtual EPROM chip.
     *
     * @param address The starting address in the chip from where the data will be read.
     * @param length  The length of the data to be read from the chip.
     * @return        The raw data read from the chip.
     */
    std::string read_raw(size_t address, size_t length)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error: Failed to open vEPROM file: " << filename << std::endl;
            return "";
        }

        file.seekg(address);
        std::string data(length, '\0');
        file.read(&data[0], length);

        file.close();
        return data;
    }

public:
    VirtualEPROM() : capacity(0) {}

    /**
     * @brief Creates a new vEPROM file with the specified capacity.
     *
     * @param cap  The capacity of the vEPROM chip in bytes.
     */
    void create(size_t cap)
    {
        capacity = cap;
        filename = "veprom_" + std::to_string(cap) + ".bin";

        if (fileExists(filename))
        {
            std::cerr << "File has already existed: " << filename << std::endl;
            return;
        }

        std::ofstream file(filename, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error: Failed to create vEPROM file\n";
            return;
        }
        // Write header
        file.write(reinterpret_cast<const char *>(&capacity), sizeof(size_t));
        std::cout << "Created vEPROM with capacity: " << capacity << " bytes\n";

        file.close();
    }

    /**
     * @brief Loads an existing vEPROM file.
     *
     * @param fn_l The filename of the vEPROM file to load.
     */
    void load(const std::string &fn_l)
    {
        std::ifstream file(fn_l, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error: Failed to load vEPROM file: " << fn_l << std::endl;
            std::cout << "Current loaded vEPROM file: " << filename << std::endl;
            return;
        }

        filename = fn_l;

        file.read(reinterpret_cast<char *>(&capacity), sizeof(size_t));
        std::cout << "Loaded vEPROM with capacity: " << capacity << " bytes\n";
        std::cout << "File path: " << filename << std::endl;

        file.close();
    }

    /**
     * @brief Erases all files and their contents from the vEPROM chip.
     *
     */
    void erase()
    {
        std::cout << "Are you sure you want to erase the virtual EPROM? This will remove all files and their contents. (Y/N): ";
        char confirmation;
        std::cin >> confirmation;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirmation != 'Y' && confirmation != 'y')
        {
            std::cout << "Erase operation aborted.\n";
            return;
        }

        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            std::cerr << "Error: Failed to open vEPROM file: " << filename << std::endl;
            return;
        }

        // Write header
        file.write(reinterpret_cast<const char *>(&capacity), sizeof(size_t));
        std::cout << "EPROM erased. Capacity retained: " << capacity << " bytes\n";

        file.close();
    }

    /**
     * @brief Lists all files stored in the vEPROM chip along with their start position and length.
     *
     * @return std::vector<std::string>  A vector of strings containing the filenames of the files stored in the vEPROM chip.
     */
    std::vector<std::string> list()
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error: Failed to open vEPROM file:" << filename << std::endl;
            return std::vector<std::string>();
        }

        // Read capacity
        file.read(reinterpret_cast<char *>(&capacity), sizeof(size_t));
        std::cout << "Capacity: " << capacity << " bytes(" << capacity - std::filesystem::file_size(filename) << " available)\n";

        auto names = std::vector<std::string>();

        FileDescriptor fd;
        while (file.read(reinterpret_cast<char *>(&fd), sizeof(FileDescriptor)))
        {
            std::string fn(fd.fname); // Convert char array to string
            std::cout << "Filename: " << fn << ", Start: " << fd.start << ", Length: " << fd.length << std::endl;

            names.push_back(fn);

            // std::string body = read_raw(fd.start + sizeof(FileDescriptor), fd.length);
            // std::cout << body << std::endl;

            file.seekg(fd.length, std::ios::cur);
        }

        file.close();

        return names;
    }

    /**
     * @brief Writes a file to the vEPROM chip.
     *
     * @param filepath The path to the file to be written to the vEPROM chip.
     */
    void write(const std::string &filepath)
    {
        if (!fileExists(filepath))
        {
            std::cerr << "File doesn't exist:" << filepath << std::endl;
            return;
        }

        if (std::filesystem::file_size(filepath) + std::filesystem::file_size(filename) + sizeof(FileDescriptor) > capacity)
        {
            std::cerr << "No enough space to write file: " << filepath << std::endl;
            return;
        }

        std::ifstream source(filepath, std::ios::binary);
        if (!source)
        {
            std::cerr << "Error: Failed to open source file for reading:" << filepath << std::endl;
            return;
        }

        // Write file name and contents to virtual EPROM chip
        std::string file_contents((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
        std::string fn_w = filepath.substr(filepath.find_last_of("/\\") + 1); // Extract file name from path
        size_t start_pos;

        // Open .bin file in append mode
        std::ofstream veprom_file(filename, std::ios::binary | std::ios::app);
        if (!veprom_file)
        {
            std::cerr << "Error: Failed to open vEPROM file for writing:" << filename << std::endl;
            return;
        }

        // Write file descriptor to the end of the file
        veprom_file.seekp(0, std::ios::end);
        start_pos = veprom_file.tellp();

        FileDescriptor fd;
        memset(fd.fname, 0, sizeof(fd.fname));                 // Initialize file name buffer
        strncpy(fd.fname, fn_w.c_str(), sizeof(fd.fname) - 1); // Copy file name to buffer

        fd.start = start_pos;
        source.seekg(0, std::ios::end); // Move to end of source file to get length

        fd.length = source.tellg();
        veprom_file.write(reinterpret_cast<const char *>(&fd), sizeof(FileDescriptor));

        // // Append contents of source file to the end of the veprom_file
        // source.seekg(0, std::ios::beg); // Move back to the beginning of the source file
        // veprom_file << source.rdbuf();  // Append contents to the virtual EPROM file
        // source.close();

        int endOfFile = veprom_file.tellp();
        veprom_file.close();
        // Write file contents using write_raw method
        write_raw(endOfFile, file_contents);

        std::cout << "File written to vEPROM: " << fn_w << ", postion: " << fd.start << ", length: " << fd.length << std::endl;
    }

    /**
     * @brief Reads the contents of a file from the vEPROM chip.
     *
     * @param fn_r The filename of the file to be read from the vEPROM chip.
     * @return std::string   A string containing the contents of the file read from the vEPROM chip.
     */
    std::string read(const std::string &fn_r)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error: Failed to open vEPROM file:" << filename << std::endl;
            return "";
        }

        // Seek past the header
        file.seekg(sizeof(size_t), std::ios::beg);

        // Find the file in the virtual EPROM
        bool found = false;
        std::string file_contents = "";
        FileDescriptor fd;
        while (file.read(reinterpret_cast<char *>(&fd), sizeof(FileDescriptor)))
        {
            std::string fn(fd.fname);

            // std::cout << fn << std::endl;

            if (fn == fn_r)
            {
                // Convert char array to string
                file_contents = read_raw(fd.start + sizeof(FileDescriptor), fd.length); // Read contents

                found = true;
                break;
            }

            file.seekg(fd.length, std::ios::cur);
        }
        file.close();

        if (!found)
        {
            std::cerr << "Error: File not found in vEPROM:" << fn_r << std::endl;
        }

        return file_contents;
    }

    /**
     * @brief Prints a help message describing available commands.
     *
     */
    static void help()
    {
        std::cout << "Commands:\n"
                  << "  create <capacity> :\n\t Create a new virtual EPROM chip\n"
                  << "  load <filename> :\n\t Load an existing virtual EPROM chip\n"
                  << "  write <file_path> :\n\t Write a file to the virtual EPROM chip\n"
                  << "  list :\n\t List files on the virtual EPROM chip\n"
                  << "  read <file> :\n\t Read a file from the virtual EPROM chip\n"
                  << "  erase :\n\t Erase the virtual EPROM chip\n"
                  << "  ui :\n\t Start the gui tool\n"
                  << "  help :\n\t Display this help message\n";
    }

    // Execute command
    void execute_command(const std::string &command)
    {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        if (cmd == "create")
        {
            size_t capacity;
            if (iss >> capacity)
                create(capacity);
            else
                std::cerr << "Usage: create <capacity>\n";
        }
        else if (cmd == "load")
        {
            std::string fname;
            if (iss >> fname)
                load(fname);
            else
                std::cerr << "Usage: load <filename>\n";
        }
        else if (cmd == "erase")
        {
            erase();
        }
        else if (cmd == "write_raw") // For test only
        {
            size_t address;
            std::string data;
            if (iss >> address >> data)
                write_raw(address, data);
            else
                std::cerr << "Usage: write_raw <address> <data>\n";
        }
        else if (cmd == "read_raw") // For test only
        {
            size_t address, length;
            if (iss >> address >> length)
                std::cout << read_raw(address, length) << std::endl;
            else
                std::cerr << "Usage: read_raw <address> <length>\n";
        }
        else if (cmd == "write")
        {
            std::string filepath;
            if (iss >> filepath)
                write(filepath);
            else
                std::cerr << "Usage: write <file_path>\n";
        }
        else if (cmd == "list")
        {
            list();
        }
        else if (cmd == "read")
        {
            std::string file;
            if (iss >> file)
                std::cout << read(file) << std::endl;
            else
                std::cerr << "Usage: read <file>\n";
        }
        else if (cmd == "help")
        {
            help();
        }
        else if (cmd == "ui")
        {
            auto ui = AppWindow::create();

            ui->on_create_veprom_file([&]
                                      { std::string fileSizeStr = "0";
                                        fileSizeStr += trim(ui->get_veprom_file_size().data());
                                        create(std::stol(fileSizeStr)); 
                                        std::cout << std::endl; });

            ui->on_load_veprom_file([&]
                                    {
                                        load(trim(ui->get_filePath().data()));
                                        // auto fnl = list();
                                        // ui->set_fileNames();
                                        std::cout << std::endl; });
            ui->on_list_veprom_file([&]
                                    {
                                        std::vector<std::string> files = list();
                                        std::string combinedString;
                                        for (const auto& file : files) {
                                            combinedString += std::string_view(file + "\n");
                                        }
                                        ui->set_fileContents(std::string_view(combinedString)); std::cout << std::endl; });
            ui->on_write_file_to_veprom([&]
                                        { write(trim(ui->get_fileToWrite().data())); std::cout << std::endl; });

            // ui->on_read_a_file([&]
            //                    { ui->set_fileContents(std::string_view(read(ui->get_fn_to_read().data()))); });
            ui->on_read_file_from_veprom([&]
                                         { ui->set_fileContents(std::string_view(read(trim(ui->get_fileToRead().data())))); std::cout << std::endl; });

            ui->run();
        }
        else
        {
            std::cerr << "Unknown command. Type 'help' for available commands.\n";
        }
    }

    ////Helper functions

    bool fileExists(const std::string &path)
    {
        struct stat statbuf;
        return stat(path.c_str(), &statbuf) == 0;
    }

    // Trim whitespace from both ends of a string
    std::string trim(const std::string &str)
    {
        size_t first = str.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string::npos)
        {
            return "";
        }

        size_t last = str.find_last_not_of(" \t\n\r\f\v");

        return str.substr(first, last - first + 1);
    }
};

int main(int argc, char *argv[])
{
    VirtualEPROM veprom;
    std::string command;

    if (argc == 2)
    {
        veprom.load(argv[1]);
    }

    std::cout << "Type 'help' for available commands.\n";
    while (true)
    {
        std::cout << ">>> ";
        std::getline(std::cin, command);
        if (command == "quit" || command == "exit")
            break;
        veprom.execute_command(command);
    }

    return 0;
}
