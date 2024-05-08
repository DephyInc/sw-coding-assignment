#include "process_cmd.h"
#include "enum.h"
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <filesystem>

using boost::lexical_cast;
using boost::bad_lexical_cast;

int n_files = 3;

class VepromFile {
    public:
        string name = "";
        int size;
        long data_start;

        friend ostream &operator<<(ostream &os, const VepromFile &file) {
            os << file.name << " " << file.size << " " << file.data_start;
            return os;
        }

        friend istream &operator>>(istream &is, VepromFile &file) {
            is >> file.name >> file.size >> file.data_start;
            return is;
        }
};

class VepromChip {
    public:
        string path;
        int size;
        long offset;
        VepromFile file[3];

        friend ostream &operator<<(ostream &os, const VepromChip &chip) {
            os << chip.path << " " << chip.size << " " << chip.offset
             << " ";
             for (int i = 0; i < n_files - 1; i++) {
                 os << chip.file[i] << " ";
             }
             os << chip.file[n_files - 1];
            return os;
        }

        friend istream &operator>>(istream &is, VepromChip &chip) {
            is >> chip.path >> chip.size >> chip.offset;
            for (int i = 0; i < n_files; i++) {
                is >> chip.file[i];
            }
            return is;
        }
};

VepromChip currentChip;

int save_chip() {
    std::fstream newChip(currentChip.path);

    if (!newChip) {
        cerr << "Could not create file!" << std::endl;
        return ReturnCodes::FOPEN_ERROR;
    }

    newChip.write((char*)&currentChip, sizeof(currentChip));
    newChip.close();
    return ReturnCodes::SUCCESS;
}

int verify_chip_open() {
    std::ifstream file(currentChip.path);

    if (!file.is_open()) {
        cerr << "Could not find a loaded veprom chip file.\n";
        cerr << "Be sure to create or load a chip file before calling write_raw\n";
        return ReturnCodes::CHIP_NOT_FOUND;
    }
    else
        return ReturnCodes::SUCCESS;
}

int process_create(vector<string> args)
{
    if (args.size() != 3) {
        cerr << "Invalid create command arguments.\n";
        cerr << "Correct usage is \"veprom create 256\"\n";
        return ReturnCodes::INVALID_CREATE;
    }

    try {
        int size = boost::lexical_cast<int>(args[2]);

        if (size <= 0) {
            cerr << "Chip size must be positive\n";
            return ReturnCodes::INVALID_SIZE;
        }
        
        currentChip.size = size;
        currentChip.path = "./chips/newChip.veprom";
        currentChip.offset = 0;
        for (int i = 0; i < n_files; i++) {
            currentChip.file[i].name = "";
        }

        std::ofstream outfile(currentChip.path);

        int save_return = save_chip();
        
        if (!save_return) {
            cout << "New veprom creation successful. Chip is stored at\n";
            cout << currentChip.path << "\n";
        }

        return save_return;
    }
    catch (bad_lexical_cast &e) {
        cerr << "Invalid size. Please enter a whole number.\n";
        cerr << e.what() << "\n";
        return ReturnCodes::INVALID_SIZE;
    }
} 

int process_load(vector<string> args) 
{
    if (args.size() != 3) {
        cerr << "Invalid load command arguments.\n";
        cerr << "Correct usage is \"veprom load /path/to/veprom/file\"\n";
        return ReturnCodes::INVALID_LOAD;
    }

    string path = args[2];
    ifstream file;
    file.open(path, ios::in);

    if (file.is_open()) {
        file >> currentChip;
        file.close();
        cout << "size: " << currentChip.size << std::endl;
        cout << "path: " << currentChip.path << std::endl;
        cout << "Successfully found the veprom file\n";
        return ReturnCodes::SUCCESS;
    }
    else {
        cerr << "Could not find veprom file to load\n";
        return ReturnCodes::CHIP_NOT_FOUND;
    }
} 

int write_helper(long addr, string contents) {
    int verify_return = verify_chip_open();
    if (verify_return)
        return verify_return;

    if (addr + contents.size() > currentChip.size) {
        cerr << "Write would go past end of file if executed, please retry\n";
        return ReturnCodes::INVALID_ADDRESS;
    }

    // This is a bit messy, may try to do metadata some other way
    int offsetLocation = addr + sizeof(currentChip);

    cout << "Writing to actual offset: " << offsetLocation << std::endl;
    cout << "Writing " << sizeof(contents) << " to file" << std::endl;
    
    std::fstream fileOut(currentChip.path);
    if (!fileOut) {
        cerr << "Could not write to file!";
        return ReturnCodes::FOPEN_ERROR;
    }

    fileOut.seekp(offsetLocation, ios::beg);
    fileOut << contents;
    fileOut.close();

    return ReturnCodes::SUCCESS;
}

/*
 * This assumes that the address can be written to, and overwritten
 * As such it does not update offset
 */
int process_write_raw(vector<string> args) 
{
    if (args.size() != 4) {
        cerr << "Invalid write_raw command arguments.\n";
        cerr << "Correct usage is \"veprom write_raw $ADDRESS $STRING\"\n";
        return ReturnCodes::INVALID_WRITE_RAW;
    }
    
    try {
        int addr = boost::lexical_cast<int>(args[2]);
        return write_helper(addr, args[3]);
    }
    catch (bad_lexical_cast &e) {
        cerr << "Invalid address. Please enter a whole number.\n";
        cerr << e.what() << "\n";
        return ReturnCodes::INVALID_ADDRESS;
    }
} 

int read_helper(long addr, int length) {
    int verify_return = verify_chip_open();
    if (verify_return)
        return verify_return;

    if (addr > currentChip.size) {
        cerr << "Cannot read past end of chip, please retry\n";
        return ReturnCodes::INVALID_ADDRESS;
    }

    // This is a bit messy, may try to do metadata some other way
    int offsetLocation = addr + sizeof(currentChip);
    cout << "Size of chip" << sizeof(currentChip) << std::endl;
    cout << "About to read from actual location: " << offsetLocation << std::endl;
    
    std::ifstream fileIn(currentChip.path, std::ios::in);
    if (!fileIn) {
        cerr << "Could not read from file!";
        return ReturnCodes::FOPEN_ERROR;
    }

    string contents(length, '\0');
    fileIn.seekg(offsetLocation);
    fileIn.read(&contents[0], length); 
    fileIn.close();

    cout << "Successfully read following from file:" << std::endl;
    cout << contents << std::endl;

    return ReturnCodes::SUCCESS;
}

int process_read_raw(vector<string> args) 
{
    if (args.size() != 4) {
        cerr << "Invalid read_raw command arguments.\n";
        cerr << "Correct usage is \"veprom read_raw $ADDRESS $length\"\n";
        return ReturnCodes::INVALID_READ_RAW;
    }
    
    try {
        int addr = boost::lexical_cast<int>(args[2]);
        int length = boost::lexical_cast<int>(args[3]);
        return read_helper(addr, length);
    }
    catch (bad_lexical_cast &e) {
        cerr << "Invalid addr/length. Please enter a whole number.\n";
        cerr << e.what() << "\n";
        return ReturnCodes::INVALID_READ_RAW;
    }
} 

int find_available_file() {
    int index = -1;

    for (int i = 0; i < n_files; i++) {
        if (currentChip.file[i].name == "") {
            return i;
        }
    }

    return index;
}

int process_write(vector<string> args) 
{
    if (args.size() != 3) {
        cerr << "Invalid write command arguments.\n";
        cerr << "Correct usage is \"veprom write path/to/file\"\n";
        return ReturnCodes::INVALID_WRITE;
    }
  
    int verify_return = verify_chip_open();
    if (verify_return)
        return verify_return;

    int index = find_available_file();

    if (index == -1) {
        cerr << "All file slots are in use" << std::endl;
        return ReturnCodes::CHIP_FILES_FULL;
    }

    filesystem::path p(args[2]); 

    std::ifstream in_file(p.filename());
    
    if (!in_file.is_open()) {
        cerr << "Could not open input file.\n";
        return ReturnCodes::FOPEN_ERROR;
    }
    
    string content( (std::istreambuf_iterator<char>(in_file) ),
                    (std::istreambuf_iterator<char>()) );
    cout << "About to try to write: " << content << std::endl;
    in_file.close();
    
    currentChip.file[index].name = p.filename();
    currentChip.file[index].size = sizeof(content);
    currentChip.file[index].data_start = currentChip.offset;
    currentChip.offset += sizeof(content);

    int save_return = save_chip();
    if (save_return)
        return save_return;
    
    int write_return = write_helper(currentChip.offset - sizeof(content), content);

    if (write_return)
        return write_return;

    return ReturnCodes::SUCCESS;
} 

int process_list(vector<string> args) 
{
    int verify_return = verify_chip_open();
    if (verify_return)
        return verify_return;
   
    for (int i = 0; i < n_files; i++) {
        if (currentChip.file[i].name == "") {
            cout << "File slot " << (i + 1) << ": [Empty]" << std::endl;
        }
        else {
            cout << "File slot " << (i + 1) << ": " << currentChip.file[i].name << std::endl;
        }
    }

    return ReturnCodes::SUCCESS;
} 

int process_read(vector<string> args)
{
    if (args.size() != 3) {
        cerr << "Invalid read command arguments.\n";
        cerr << "Correct usage is \"veprom read file\"\n";
        return ReturnCodes::INVALID_READ;
    }
  
    int verify_return = verify_chip_open();
    if (verify_return)
        return verify_return;

    int file_index = -1;
    for (int i = 0; i < n_files; i++) {
        if (currentChip.file[i].name == args[2]) {
            file_index = i;
            break;
        }
    }

    cout << "Reading from file index " << file_index << std::endl;

    if (file_index == -1) {
        cerr << "File not on chip" << std::endl;
        return ReturnCodes::CHIP_FILE_NOT_FOUND;
    }

    cout << "File data start at " << currentChip.file[file_index].data_start << std::endl;
    cout << "File size " << currentChip.file[file_index].size << std::endl;

    int read_return = read_helper(currentChip.file[file_index].data_start, currentChip.file[file_index].size);    
    if (read_return)
        return read_return;

    return ReturnCodes::SUCCESS;
} 

/*
 * This is not intended to be a hard erase, but marks memory as available instead
 */
int process_erase(vector<string> args)
{
    int verify_return = verify_chip_open();
    if (verify_return)
        return verify_return;

    currentChip.offset = 0;
    for (int i = 0; i < n_files; i++)
        currentChip.file[i].name = "";
    
    int save_return = save_chip();
    
    if (!save_return) {
        cout << "Chip " << currentChip.path << " has been cleared" << std::endl;
        return ReturnCodes::SUCCESS;
    }
    else {
        return save_return;
    }
}

int process_help()
{
    cout << "Invalid command. Here are some guidelines:\n";
    cout << "create: creates a new virtual EPROM chip with a specified capacity.\n";
    cout << "load: loads a vEPROM file.\n";
    cout << "write_raw: writes a raw string of bytes to a specific address on the virtual EPROM chip.\n";
    cout << "read_raw: reads the values stored at a specific address and length on the virtual EPROM chip and outputs it on stdout.\n";
    cout << "write: writes a file to the virtual EPROM chip.\n";
    cout << "list: lists the files on the virtual EPROM chip.\n";
    cout << "read: reads a file from the virtual EPROM chip.\n";
    cout << "erase: sets the EPROM back to its original state\n";

    return 0;
}
