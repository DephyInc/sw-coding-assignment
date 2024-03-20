Here's an illustration of the structure of the virtual EPROM (vEPROM) file, including the file descriptor:

```
                +-------------------+
                |    Capacity       |
                +-------------------+
                | File Descriptor 1 |
                +-------------------+
                |      File 1       |
                +-------------------+
                | File Descriptor 2 |
                +-------------------+
                |      File 2       |
                +-------------------+
                |       ...         |
                +-------------------+
```

- `Capacity`: This is the total capacity of the virtual EPROM, typically stored as a fixed-size data type like `size_t` at the beginning of the file.
- `File Descriptor N`: This section contains information about each file stored in the vEPROM. Each file descriptor includes details such as the file name, the starting position of the file's contents in the file, and the length of the file's contents.

- `File N`: These sections represent the actual contents of each file stored in the vEPROM. The file contents are stored sequentially after the file descriptors.

Each file descriptor could have the following structure:

```
                +-------------------+
                |     File Name     |
                +-------------------+
                |   Start Position  |
                +-------------------+
                |     File Length   |
                +-------------------+
```

- `File Name`: This field stores the name of the file.
- `Start Position`: This field stores the byte offset from the beginning of the vEPROM file where the contents of the file start.
- `File Length`: This field stores the length of the file's contents in bytes.

#

#

#

#

#

#

### Class: VirtualEPROM

**Description:**
A class representing a virtual EPROM (vEPROM) emulator that allows users to simulate reading, writing, listing, and erasing files stored in a virtual EPROM chip.

**Member Variables:**

- `filename`: A string representing the filename of the vEPROM file.
- `capacity`: An unsigned integer representing the total capacity of the vEPROM chip in bytes.

**Member Functions:**

1. `create(size_t cap)`

   - **Description:** Creates a new vEPROM file with the specified capacity.
   - **Parameters:**
     - `cap`: The capacity of the vEPROM chip in bytes.
   - **Returns:** void

2. `load(const std::string &fn_l)`

   - **Description:** Loads an existing vEPROM file.
   - **Parameters:**
     - `fn_l`: The filename of the vEPROM file to load.
   - **Returns:** void

3. `erase()`

   - **Description:** Erases all files and their contents from the vEPROM chip.
   - **Parameters:** None
   - **Returns:** void

4. `list()`

   - **Description:** Lists all files stored in the vEPROM chip along with their start position and length.
   - **Parameters:** None
   - **Returns:** A vector of strings containing the filenames of the files stored in the vEPROM chip.

5. `write(const std::string &filepath)`

   - **Description:** Writes a file to the vEPROM chip.
   - **Parameters:**
     - `filepath`: The path to the file to be written to the vEPROM chip.
   - **Returns:** void

6. `read(const std::string &fn_r)`

   - **Description:** Reads the contents of a file from the vEPROM chip.
   - **Parameters:**
     - `fn_r`: The filename of the file to be read from the vEPROM chip.
   - **Returns:** A string containing the contents of the file read from the vEPROM chip.

7. `help()`

   - **Description:** Prints a help message describing available commands.
   - **Parameters:** None
   - **Returns:** void

8. `execute_command(const std::string &command)`

   - **Description:** Executes a command entered by the user.
   - **Parameters:**
     - `command`: The command to be executed.
   - **Returns:** void

9. `fileExists(const std::string &path)`

   - **Description:** Checks if a file exists.
   - **Parameters:**
     - `path`: The path to the file.
   - **Returns:** A boolean value indicating whether the file exists.

10. `trim(const std::string &str)`

- **Description:** Trims whitespace from both ends of a string.
- **Parameters:**
  - `str`: The string to be trimmed.
- **Returns:** A string with whitespace trimmed from both ends.

### Main Function

**Description:**
The main function of the application, which initializes a `VirtualEPROM` object and processes user commands entered via the command line.

**Parameters:**

- `argc`: An integer representing the number of command-line arguments.
- `argv`: An array of strings representing the command-line arguments.

**Returns:** An integer representing the exit status of the program.

#

#

#

#

#

#

### How to build

## Prerequisites

In order to build this C++ application, you need to install a few tools:

- cmake (3.21 or newer)
- A C++ compiler that supports C++ 20
- install cargo

If your target environment is Linux or Windows on an x86-64 architecture, then you may also opt into downloading one of our binary Slint packages. These are pre-compiled and require no further tools. You can find setup instructions and download links at [Slint Documentation](https://slint.dev/docs/cpp/cmake.html#install-binary-packages)

Alternatively, this template will automatically download the Slint sources and compile them. This option requires you to install Rust by following the Rust Getting Started Guide. Once this is done, you should have the rustc compiler and the cargo build system installed in your path.

## Build

1. Clone or download this repository
   ```
   git clone CHANGEME my-project-cpp
   cd my-project-cpp
   ```
2. Configure with CMake
   ```
   mkdir build
   cmake -B build
   ```
3. Build with CMake
   ```
   cmake --build build
   ```
4. Run the application binary
   - Linux:
     ```
     ./build/my_application
     ```
   - Windows:
     ```
     build\debug\my_application.exe
     ```

###

## Dev environment:

- windows 11, MSVC ISO C++20 Standard (/std:c++20)

## Test environments:

- windows 11, Linux PopOS

## Note:

On Dell Latitude D620(an old laptop):

      ```
      tiger@pop-os:~/.hyh/embedded/2024/Dephy/slint/slint_test/my-project-cpp$ ./build/my_application
      Type 'help' for available commands.
      >>> ui
      MESA-LOADER: failed to open i915: /usr/lib/dri/i915_dri.so: cannot open shared object file: No such file or directory (search paths /usr/lib/x86_64-linux-gnu/dri:\$${ORIGIN}/dri:/usr/lib/dri, suffix _dri)
      failed to load driver: i915
      ```

Slint gui still works.
