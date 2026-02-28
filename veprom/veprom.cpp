// veprom.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <fstream>
#include <iostream>

class veprom {

public:
    char* epromLocalImage = NULL;
    const char* epromStoredPathName = "activeImageName.tmp";
    char* epromImageFileName = NULL;
    uint32_t epromSize = 0;


    /// <summary>
    /// Starts the virtual eprom class.  Due to the simplicity of the command line interface as specified,
    /// the capacity to work multiple commands necessitates the use of a temporary file so that a return
    /// to the command line (and the ability to build on the eprom in the next command)
    /// </summary>
    veprom() {

        std::ifstream curpath(epromStoredPathName);
        if (curpath.good()) {
            // collect size; populate epromSize;
            curpath.seekg(0, std::ios_base::end);
            std::streamoff pathSize = curpath.tellg();
            curpath.seekg(0, std::ios_base::beg);
            // allocate buffer image and populate it with the last saved image
            epromImageFileName = new char[256] {};
            curpath.read(epromImageFileName, 256);
            curpath.close();
        }
        if (epromImageFileName != NULL) {
            std::ifstream tmpimg(epromImageFileName);
            if (tmpimg.good()) {
                // collect size; populate epromSize;
                tmpimg.seekg(0, std::ios_base::end);
                epromSize = (uint32_t)tmpimg.tellg();
                tmpimg.seekg(0, std::ios_base::beg);
                // allocate buffer image and populate it with the last saved image
                epromLocalImage = new char[epromSize];
                memset((void*)epromLocalImage, 0xff, epromSize);
                tmpimg.read(epromLocalImage, epromSize);
                tmpimg.close();
            }
        }
    }
    /// <summary>
    /// Inside this destructor, the file saving the working buffer is written, along with the freeing of the memory allocated for it.
    /// </summary>
    ~veprom() {
        if ((epromSize > 0) && (epromImageFileName != NULL)) {
            std::ofstream tmpimg(epromImageFileName);
            if (tmpimg.good()) {
                tmpimg.write(epromLocalImage, epromSize);
            }
        }
        delete[] epromLocalImage;
        delete[] epromImageFileName;
    }
    /// <summary>
    /// This method creates the buffer for a fresh, unwritten, eprom image in the number of kbytes specified in the string
    /// </summary>
    /// <param name="kbyteSizeString">The number of kbytes (1k==1024 bytes) for the virtual eprom</param>
    void create(const char* kbyteSizeString) {
        epromSize = std::atoi(kbyteSizeString) * 1024;
        if (epromLocalImage != NULL) {
            delete[] epromLocalImage;
            epromLocalImage = NULL;  // you just never know what the next dude does.
        }
        epromLocalImage = new char[epromSize];
        eraseBuffer();
        std::cout << "eprom image was created with a size of " << kbyteSizeString << "k, or " << epromSize << " bytes\n";
    }

    /// <summary>
    /// This method simulates the erasure of a UV eprom, which in its erased state is all 1's
    /// </summary>
    void eraseBuffer() {
        memset((void*)epromLocalImage, 0xff, epromSize);
    }


    void SetSavedImgPath(char* pathName) {
        std::ofstream tmppth(epromStoredPathName);
        if (tmppth.good()) {
            tmppth.write(pathName, strlen(pathName));
        }
        tmppth.close();
    }
    /// <summary>
    /// This method will write bytes to a particular place in the eprom image buffer at a particular place
    /// </summary>
    /// <param name="address">This is the byte offset into the buffer</param>
    /// <param name="string">This is the data to be written</param>
    void writeRaw(int address, const char* string) {
        size_t stringsize = strlen(string);
        memcpy_s((void*)(epromLocalImage + address), (epromSize - address), string, stringsize);
    }
    /// <summary>
    /// This method puts out the stdout a block of memory from the eprom buffer.  non-printable characters are converted into hexadecimal notation
    /// </summary>
    /// <param name="address">The starting offset into the buffer for the data to extract</param>
    /// <param name="length">The length of the data buffer to be extracted</param>
    void readRaw(uint32_t address, uint32_t length) {
        if (length < epromSize) {
            char* bufptr = (epromLocalImage + address);
            if (length + address < (int)epromSize) {
                uint32_t counter = 0;
                while (counter < length)
                {
                    if ((bufptr[counter] >= ' ') && (bufptr[counter] <= '~')) {
                        std::cout << bufptr[counter];
                    }
                    else {
                        // give something readable if it isn't a printable character.
                        printf("\\x%x", (uint8_t)bufptr[counter]);
                    }
                    counter++;
                }
                std::cout << std::endl;
            }
        }
    }


    // The file format inside the eprom looks like this:
    // <4 bytes specifying the next file's start><1 byte for filename length><file data>
    // For example a first file could look something like this:
    // "\x31\x00\x00\x00\x14ExamplefileName1.txtExample File Image"
    // The bytes "\x31\x00\x00\x00" represent the 32 bit unsigned offset value to the start of the next file
    // The byte "\x14" is the length of the filename (a.k.a. 20 bytes long.
    // making the file's text start at 25 bytes after the start of the header, and running to the start of the next file

    /// <summary>
    /// This method steps through the files and returns with the offset to the end of the written part of the eprom.
    /// In other words, the first open spot.
    /// </summary>
    /// <returns>The offset to the first open file starting point</returns>
    uint32_t findNextOpen() {
        uint32_t nextFileStartPoint = 0;
        uint32_t currentFileStartPoint = 0;
        //plug the next starting address for the next file into the eprom
        while (nextFileStartPoint < epromSize) {
            currentFileStartPoint = nextFileStartPoint;
            nextFileStartPoint = *(uint32_t*)(epromLocalImage + nextFileStartPoint);
        }
        return currentFileStartPoint;
    }

    /// <summary>
    /// This method adds another "file" to the eprom
    /// </summary>
    /// <param name="pathName">pathname to the file to be stored</param>
    void writeFile(char* pathName) {
        size_t pathLen = strlen(pathName);
        uint32_t fileOffset = findNextOpen();
        if (pathLen < 256) {
            std::ifstream tmpimg(pathName);
            if (tmpimg.good()) {
                // collect size of the file body to write;
                tmpimg.seekg(0, std::ios_base::end);
                std::streamoff filelen = tmpimg.tellg();
                tmpimg.seekg(0, std::ios_base::beg);
                uint32_t fileBodyStart = fileOffset + 5 + pathLen;
                uint32_t nextStart = fileBodyStart + filelen;
                if (nextStart < epromSize) {
                    //plug the next starting address for the next file into the eprom
                    *(uint32_t*)(epromLocalImage + fileOffset) = (uint32_t)(nextStart);
                    // plug the length of the filename into the eprom
                    *(uint8_t*)((epromLocalImage + fileOffset + 4)) = (uint8_t)(pathLen % 256);
                    //plug the filename into the eprom
                    memcpy_s((epromLocalImage + fileOffset + 5), (epromSize - (fileOffset + 5)), pathName, pathLen);
                    // plug in the file data
                    tmpimg.read((epromLocalImage + fileBodyStart), filelen);
                    std::cout << "File successfully added to the eprom\n";
                }
                else {
                    std::cout << "sorry, there is insufficent room to add the named file\n";
                }
                tmpimg.close();
            }
            else {
                std::cout << "sorry, there was a problem loading the named file\n";
            }

        }
        else {
            std::cout << "sorry, The path name was too long\n";
        }

    }

    /// <summary>
    /// This method catalogs all the files in the eprom image
    /// </summary>
    void listFiles() {
        uint32_t nextFileStartPoint = 0;
        uint32_t currentFileStartPoint = 0;
        char currentFileName[256] = { 0 };
        uint8_t fNameLen = 0;
        if (((*(uint32_t*)(epromLocalImage) > epromSize)) || (epromSize == 0)) {
            std::cout << "No files are present\n";
            return;
        };

        //plug the next starting address for the next file into the eprom
        while (nextFileStartPoint < epromSize) {
            currentFileStartPoint = nextFileStartPoint;
            // grab the length of the filename into the eprom
            fNameLen = *(uint8_t*)((epromLocalImage + currentFileStartPoint + 4));
            //pluck the filename into the eprom and send it out stdout
            memcpy_s(currentFileName, 255, (epromLocalImage + currentFileStartPoint + 5), fNameLen);
            if (fNameLen < 256) {
                currentFileName[fNameLen] = '\0';
            }
            std::cout << currentFileName << std::endl;
            nextFileStartPoint = *(uint32_t*)(epromLocalImage + nextFileStartPoint);
        }
    }

    /// <summary>
    /// This method, given the filename, produces the contents for that file.
    /// </summary>
    /// <param name="pathName">the filename, as stored in the eprom image</param>
    void readfile(char* pathName) {
        size_t pathLen = strlen(pathName);
        uint32_t nextFileStartPoint = 0;
        uint32_t currentFileStartPoint = 0;
        char currentFileName[256] = { 0 };
        uint8_t fNameLen = 0;
        //plug the next starting address for the next file into the eprom
        while (nextFileStartPoint < epromSize) {
            currentFileStartPoint = nextFileStartPoint;
            nextFileStartPoint = *(uint32_t*)(epromLocalImage + currentFileStartPoint);
            // grab the length of the filename into the eprom
            fNameLen = *(uint8_t*)((epromLocalImage + currentFileStartPoint + 4));
            //plug the filename into the eprom
            if (_strnicmp((epromLocalImage + currentFileStartPoint + 5), pathName, fNameLen) == 0) {
                uint32_t fileBodyCurrentChar = currentFileStartPoint + 5 + pathLen;
                while ((fileBodyCurrentChar < nextFileStartPoint) && (fileBodyCurrentChar < epromSize)) {
                    std::cout << *(epromLocalImage + fileBodyCurrentChar);
                    fileBodyCurrentChar++;
                }
                std::cout << std::endl;
                return;
            }
        }
        if (nextFileStartPoint > epromSize) {
            std::cout << "No match for that filename in the eprom, check for presence with the list command\n";
        }

    }

};

int main(int argc, char* argv[])
{
    auto my_eprom = new veprom();

    if (my_eprom->epromImageFileName == NULL) {
        if (_strnicmp(argv[1], "load", 4) == 0) {
            my_eprom->SetSavedImgPath(argv[2]);
        }
        else {
            std::cout << "Please set the eprom filename before any other actions\n";
        }
    }
    else if (_strnicmp(argv[1], "create", 7) == 0) {
        my_eprom->create(argv[2]);
    }
    else if (_strnicmp(argv[1], "load", 4) == 0) {
        my_eprom->SetSavedImgPath(argv[2]);
    }
    else if (_strnicmp(argv[1], "write_raw", 9) == 0) {
        int offset = atoi(argv[2]);
        my_eprom->writeRaw(offset, argv[3]);
    }
    else if (_strnicmp(argv[1], "read_raw", 8) == 0) {
        int offset = atoi(argv[2]);
        int len = atoi(argv[3]);
        my_eprom->readRaw(offset, len);
    }
    else if (_strnicmp(argv[1], "write", 5) == 0) {
        my_eprom->writeFile(argv[2]);
    }
    else if (_strnicmp(argv[1], "list", 4) == 0) {
        my_eprom->listFiles();
    }
    else if (_strnicmp(argv[1], "read", 4) == 0) {
        my_eprom->readfile(argv[2]);
    }
    else if (_strnicmp(argv[1], "erase", 5) == 0) {
        my_eprom->eraseBuffer();
    }
    else {
        std::cout << "Im sorry,the command you entered was not recognized\n"
            << "Valid commands are:\n"
            << "create nn where nn is the number of kilobytes to make the eprom]\n"
            << "write_raw to force specific bytes to a particular eprom memory address\n"
            << "read_raw to output a particular set of bytes in the eprom image\n"
            << "write to write a text file intoi the eprom\n"
            << "list to list the text file(s) in the eprom\n"
            << "read to report a file from the eprom\n"
            << "erase to blank out the eprom\n";
    }

    delete my_eprom;
    return 0;
}

