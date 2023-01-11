# Dephy Coding Assignment

## EPROM Emulator

Design and implement an EPROM emulator that can read and write files to virtual EPROM chips (vEPROM).

An EPROM (Erasable Programmable Read-Only Memory) is a type of non-volatile memory that can be programmed (written to) an unlimited number of times. Once programmed, they can only be erased (totally) by exposing them to UV light.

### The emulator should have the following features:

* Create a new virtual EPROM chips with a specified capacity (e.g. 256 KB).
* Read and write files or arbitrary data to the virtual EPROM chips.
* List the files on a virtual EPROM chip.
* Erase the virtual EPROM chip.

The vEPROM emulator should be implemented as a command line tool, with the following commands:

* `create`: creates a new virtual EPROM chip with a specified capacity.
  * Usage: `veprom create 256` creates a new virtual EPROM chip with a capacity of 256 KB. It outputs the path of the file used to store the vEPROM. This is the vEprom that’ll be used when the other commands are called.
* `load`: loads a vEPROM file
  * Usage: `veprom load /path/to/veprom/file`. This is the vEprom that’ll be used when the other commands are called.
* write_raw: writes a raw string of bytes to a specific address on the virtual EPROM chip.
  * Usage: `veprom write_raw $ADDRESS $STRING`
* `read_raw`: reads the values stored at a specific address and length on the virtual EPROM chip and outputs it on stdout.
  * Usage: `veprom read_raw $ADDRESS $length`
* write: writes a file to the virtual EPROM chip.
  * Usage: `veprom write /path/to/local/file` writes the file /path/to/local/file to the virtual EPROM chip as `file`.
* list: lists the files on the virtual EPROM chip.
  * Usage: `veprom list`
* read: reads a file from the virtual EPROM chip.
  * Usage: `veprom read $FILE` reads the `$FILE` file from the virtual EPROM chip and outputs it to stdout.
* `erase`: sets the EPROM back to its original state
  * Usage: `veprom erase`

As a bonus, build a graphical front-end for the emulator that allows users to visualize how the files are stored on the virtual EPROM chip.

## Constraints
* The emulator should be implemented in C++.
* The emulator should use a flat file system to store the virtual EPROM chips. Each chip should be stored as a separate file on the local file system.
* The emulator should handle errors gracefully and provide appropriate feedback to the user (e.g. if the user tries to write a file that is larger than the capacity of the chip).

## Evaluation Criteria

1. Correctness: Does the emulator correctly implement the required features and handle errors gracefully?
2. Usability: Is the emulator easy to use and understand?
3. Code quality: Is the code well-organized, maintainable, and properly documented?
4. Bonus: Does the graphical front-end provide a useful visualization of the file system on the virtual EPROM chip?

## Deliverables

Please provide the following as part of your submission in a PR to this repo:
The source code for the emulator and graphical front-end.
Instructions for how to build and run the emulator.
A short video demonstrating the use of the emulator.

## Submitting the Solution

To submit your solution, fork this repo, create your code, and submit a Pull Request once you're ready for Dephy to take a look.
