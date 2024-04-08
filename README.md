# Dephy Coding Assignment

## EPROM Emulator

A gui version of EPROM emulator and a cli version are implemented.

In the gui version, for each command with parameters, input the parameters for the command, and click the button. After the parameters are typed, the corresponding button is enabled, we can click the button to run the command. Different parameters are separated by space (' ').

In the cli version, each command with its parameters must be input in one line. The command keys and parameters in a line are separated by space.

Both versions have the following features:

* Create multiple virtual EPROM chips (vEPROMs), each with a specified capacity (e.g. 256 KB).
* Read/write files or arbitrary data from/to the vEPROMs.
* List the vEPROMs created and the files on the current vEPROM loaded.
* Erase a vEPROM.

### The cli version of the emulator implemented the following commands:

* `create`: creates a new vEPROM with a specified capacity.
  * Usage: `veprom create 256` creates a new vEPROM with a capacity of 256 KB. It outputs the path of the file used to store the vEPROM. Using create command multiple times will create multiple new vEPROMs. One vEPROM will be selected as the current vEPROM that’ll be used when the other commands are called. The maximum capacity allowed for a vEPROM is 1000 KB. If the given capacity is bigger than the maximum, the vEPROM will not be created and message "Capacity is too big." will be displayed.
* `load`: loads a vEPROM file as a current vEPROM.
  * Usage: `veprom load /path/to/veprom/file` sets the vEPROM (file) as the current vEPROM that’ll be used when the other commands are called. If the file does not exist, message "Wrong vEPROM command (file incorrect)" will be displayed.
* `write_raw`: writes a raw string to a specific address in the current vEPROM.
  * Usage: `veprom write_raw $ADDRESS $STRING`. If the string to be written at the address is outside of the vEPROM, nothing will be written and message "wrtie_raw out of vEPROM range" will be displayed.
* `read_raw`: reads and outputs the values stored at a specific address for length bytes in the current vEPROM.
  * Usage: `veprom read_raw $ADDRESS $length`. If the values are outside of the current vEPROM, nothing will be read and message "read_raw out of vEPROM range" will be displayed.
* `write`: writes a file to the current vEPROM.
  * Usage: `veprom write /path/to/local/file` writes the file /path/to/local/file to the current vEPROM as `file`. When using write command multiple times to write multiple files, these files are concatenated in order in the vEPROM (i.e., the first file is written to address 0, the second file follows the first one, the third file follows the second one, and so on). If there is not enough room for the file, the file will not be written and message "File is too big." will be displayed.
* `list`: lists the vEPROMs created and the files in the current vPROM.
  * Usage: `veprom list`. If no vEPROM is loaded/selected as current working vEPROM, message "No vEPROM is loaded/selected as current vEPROM" will be displayed.
* `read`: reads a file from the current vEPROM.
  * Usage: `veprom read $FILE` reads the `$FILE` file from the current EPROM and outputs it to stdout. If the file is not in the vEPROM, message "File not in vEPROM" will be displayed.
* `erase`: sets the current vEPROM back to its original state.
  * Usage: `veprom erase`.  If no vEPROM is loaded/selected as current working vEPROM, message "No vEPROM/file loaded" will be displayed.
* `quit`: exits the simulator.
  * Usage: `quit`

### The gui version of the emulator implemented the above commands using Qt.

For each command, if its parameters are not correct such as wrong type or the number of parameters is not expected, a corresponding message such as "Capacity must be a number" or "Wrong vEPROM command (# parameters incorrect)" will be displayed.

## How to build and run the emulator

### On Windows
* Go to build/ directory and run the following three commands for generating and installing cli_veprom.exe and gui_veprom.exe in bin/ directory (bin and build are at the same level) 
  * cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=../ -DCMAKE_MAKE_PROGRAM=C:/cygwin64/bin/make.exe .. 
  * cmake --build . 
  * cmake --install . 
* Go to bin/ directory to launch the simulator (cli_veprom.exe or gui_veprom.exe).

* Note: Qt (version >= 6), cmake, c++ compiler need to be installed. In case that gui simulator cannot be generated, we can overwrite CMakeLists.txt with CMakeLists-cli.txt and then Go to build/ directory and run the above three commands for generating and installing cli version simulator cli_veprom.exe in bin/ directory. If issues for building cli and gui versions are resolved, we can overwrite CMakeLists.txt with CMakeLists-cli-gui.txt (which is the same as the original CMakeLists.txt) and rebuild the two versions again.

### On Linux
* Go to build/ directory and run the following three commands for generating and installing cli_veprom.exe and gui_veprom.exe in bin/ directory (bin and build are at the same level) 
  * sudo cmake .. 
  * sudo cmake --build . 
  * sudo cmake --install . 
* Go to bin/ directory to launch the simulator (cli_veprom.exe or gui_veprom.exe).

* Note: Qt (version >= 6), cmake, c++ compiler need to be installed. In case that gui simulator cannot be generated, we can overwrite CMakeLists.txt with CMakeLists-cli.txt and then Go to build/ directory and run the above three commands for generating and installing cli version simulator cli_veprom.exe in bin/ directory. If issues for building cli and gui versions are resolved, we can overwrite CMakeLists.txt with CMakeLists-cli-gui.txt and rebuild two versions again.