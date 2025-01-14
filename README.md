**Virtual EPROM Emulator**

**1. IMPLEMENTATION**

The emulator is implemented as a command line tool in C++. It uses a simple flat file system and runs on Windows in the Visual Studio Code IDE (I currently don't have access to a Linux machine). The emulator handles errors gracefully and provides appropriate feedback to the user. There's also a graphical front-end written in Python to view the files with filename, size, and listed in their stored order on the virtual EEPROM.

Features
- create: creates a virtual EPROM chip with a specified capacity in KB.
- load: loads a virtual EPROM file.
- write_raw: writes a raw string of bytes to an address on the virtual EPROM.
- read_raw: reads values at an address with given length on the virtual EPROM.
- write: writes a file to the virtual EPROM chip.
- read:  reads a file from the virtual EPROM chip.
- list:  lists the files on the virtual EPROM chip.
- erase: sets the EPROM back to its original state.

**2. TESTING**

Run veprom.exe and veprom_gui.py under the repo test directory in Windows-based Visual Studio Code IDE.

**Print command usage.**

.\veprom

Usage:

    veprom create <capacity_in_kb>
    veprom load  /path/to/veprom/file
    veprom write /path/to/local/file
    veprom read  <file_name>
    veprom write_raw <address> <string>
    veprom read_raw  <address> <length>
    veprom list
    veprom erase

**Create a virtual EPROM with 256 KB.**

.\veprom create 256  
Created virtual EPROM: "C:\\vEPROM\\test\\veprom_256.bin"  
Data size: 262144 bytes

**Create a virtual EPROM that already exists.**

.\veprom create 256  
Error: Virtual EPROM file 'veprom_256.bin' already exists.

**Create a virtual EPROM with capacity smaller than total file header size.**  
**A flat file system is implemented for up to 128 files, 48 bytes per header.**

.\veprom create 2  
Error: Capacity is too small for the file header.

**Load an existing vEPROM file.**

.\veprom load veprom_256.bin  
Successfully loaded virtual EPROM: ""C:\\vEPROM\\test\\veprom_256.bin""  
Data size: 262144 bytes

**Load a vEPROM file that doesn't exist.**

.\veprom load veprom_512.bin  
Error: Failed to open file.

**Write raw ASCII string 'ABCD' to address 10 on vEPROM.**

.\veprom write_raw 10 ABCD

**Read raw hex values at address 10 on vEPROM.**  
**Unwritten vEPROM values are initialized to 0xFF.**

.\veprom read_raw 10 8  
Data at requested range:  
41 42 43 44 ff ff ff ff

**Write raw address out of range error.**  
**The valid range is 0 to 262144 for 256 KB.**

.\veprom write_raw 262145 1A  
Error: Address out of range.

**Read raw address out of range error.**  
**The valid range is 0 to 262144 for 256 KB.**

.\veprom read_raw 262145 2  
Error: Address out of range.

**Write raw out of boundary, i.e. addr + data size > capacity.**

.\veprom write_raw 262140 12345ABCDE  
Error: Content exceeds EPROM boundary.

**Read raw out of boundary, i.e. addr + length > capacity.**

.\veprom read_raw 262140 10  
Error: Length exceeds EPROM boundary.

**Write files to virtual EPROM.**

.\veprom write .\file_1.txt  
.\veprom write .\file_2.txt  
.\veprom write .\file_3.txt

**Read files. Output content on stdout.**

.\veprom read file_1.txt  
FILE 1  
.\veprom read file_2.txt  
FILE 2  
.\veprom read file_3.txt  
FILE 3  

**Write a file that doesn't exist.**

.\veprom write .\file_4.txt  
Error: Failed to open local file.

**Read a file that doesn't exist.**

.\veprom read file_4.txt  
Error: File not found on EPROM.

**Write a file whose size exceeds capacity.**

.\veprom write .\addr2line.exe  
Error: File size exceeds EPROM capacity.

**List an empty virtual EPROM.**

.\veprom list  
No files stored on EPROM.

**List a non-empty virtual EPROM.**

Files on virtual EPROM:  
  Name: file_1.txt, Size: 8 bytes  
  Name: file_2.txt, Size: 8 bytes  
  Name: file_3.txt, Size: 8 bytes  

**Erase the virtual EPROM. Verify with list to and read_raw.**

.\veprom erase  
.\veprom list  
No files stored on EPROM.  
.\veprom read_raw 10 8  
Data at requested range:  
ff ff ff ff ff ff ff ff

**To view files with GUI, run the Python veprom_gui.py program.**  
**A window will pop up. Click "Refresh" to update the list of files.**

![veprom_viewer](https://github.com/user-attachments/assets/9fcdd64f-20ba-4447-8e92-d9ac2ff42a25)

