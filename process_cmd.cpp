#include "process_cmd.h"
#include <iostream>

int process_create(vector<string> args)
{
    return 0;
} 

int process_load(vector<string> args) 
{
    return 0;
} 

int process_write_raw(vector<string> args) 
{
    return 0;
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
