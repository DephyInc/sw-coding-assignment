#include "process_cmd.h"
#include "enum.h"
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using boost::bad_lexical_cast;

string currentChipPath;

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

        ofstream newChip;
        string path = "./chips/newChip.veprom";
        newChip.open(path);

        if (!newChip) {
            cerr << "Could not create file!";
            return ReturnCodes::FOPEN_ERROR;
        }

        // Making the decision to store size like this. 
        // This may not be the best way but seemed reasonable,
        // keeping file metadata in the file itself.
        newChip << size << "\n";

        currentChipPath = path;

        cout << "New veprom creation successful. Chip is stored at\n";
        cout << path << "\n";

        newChip.close();

        return ReturnCodes::SUCCESS;
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
    std::ifstream file(path);

    if (file.is_open()) {
        cout << "Successfully found the veprom file\n";
        currentChipPath = path;
        file.close();
        return ReturnCodes::SUCCESS;
    }
    else {
        cerr << "Could not find veprom file to load\n";
        return ReturnCodes::CHIP_NOT_FOUND;
    }
} 

/*
 * This assumes that the address can be written to, and overwritten
 */
int process_write_raw(vector<string> args) 
{
    if (args.size() != 4) {
        cerr << "Invalid write_raw command arguments.\n";
        cerr << "Correct usage is \"veprom write_raw $ADDRESS $STRING\"\n";
        return ReturnCodes::INVALID_WRITE_RAW;
    }
    
    std::ifstream file(currentChipPath);

    if (!file.is_open()) {
        cerr << "Could not find a loaded veprom chip file.\n";
        cerr << "Be sure to create or load a chip file before calling write_raw\n";
        return ReturnCodes::CHIP_NOT_FOUND;
    }

    cout << "Successfully found the veprom file\n";

    try {
        int addr = boost::lexical_cast<int>(args[2]);
        string to_write = args[3];
        string line;
        file >> line;
        int chipSize = boost::lexical_cast<int>(line);
        file.close();

        if (addr + to_write.size() > chipSize) {
            cerr << "Write would go past end of file if executed, please retry\n";
            return ReturnCodes::INVALID_ADDRESS;
        }

        // This is a bit messy, may try to do metadata some other way
        int offsetLocation = addr + line.size();
        
        std::ofstream fileOut(currentChipPath, std::ios::out | std::ios::in);
        if (!fileOut) {
            cerr << "Could not write to file!";
            return ReturnCodes::FOPEN_ERROR;
        }

        fileOut.seekp(offsetLocation);
        fileOut << to_write;
        fileOut.close();

        return ReturnCodes::SUCCESS;

    }
    catch (bad_lexical_cast &e) {
        cerr << "Invalid size. Please enter a whole number.\n";
        cerr << e.what() << "\n";
        return ReturnCodes::INVALID_SIZE;
    }
} 

int process_read_raw(vector<string> args) 
{
    return 0;
} 

int process_write(vector<string> args) 
{
    return 0;
} 

int process_list(vector<string> args) 
{
    return 0;
} 

int process_read(vector<string> args)
{
    return 0;
} 

int process_erase(vector<string> args)
{
    return 0;
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
