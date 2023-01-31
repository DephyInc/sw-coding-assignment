#include <string>
#include <vector>

using namespace std;

#define SZ_FILENAME_BUF 128
#define N_VEPROMS_ALLOWED 16
#define METHOD_CREATE "create"
#define METHOD_LOAD "load"
#define METHOD_WRITE_RAW "write_raw"
#define METHOD_READ_RAW "read_raw"
#define METHOD_WRITE "write"
#define METHOD_LIST "list"
#define METHOD_READ "read"
#define METHOD_ERASE "erase"

#define FILENAME_EXT ".map"
#define FILENAME_CONTEXT "veprom_context" FILENAME_EXT

class Veprom
{
    public:

        enum eRetVal
        {
            OK = 0,
            __InputErrors__ = 0x80,
            RequireMethod,
            RequireSize,
            InvalidSize,
            SizeTooLarge,
            InvalidMethod,
            RequireLoadChoice,
            RequireAddress,
            RequireData,
            RequireLength,
            InvalidLength,
            RequireFilename,
            FileWriteNotFound,
            __AppErrors__ = 0xC0,
            FilenameBufferInvalid,
            FilenamesExhausted,
            MemoryAllocError,
            FileCreationError,
            ContextNotFound,
            CannotOpenContext,
            CannotWriteContext,
            ContextFileNotFound,
            ContextNotLoaded,
            WriteOutOfBounds,
            OpenFailedWriteRaw,
            OpenFailedReadRaw,
            ReadOutOfBounds,
            NullPtr,
            FilenameTooLong,
            WriteFileDriveFull,
            CannotGetSize,
            FileNotFound,
            __Unknown__ = 0xFF,
        };

        Veprom() {};
        ~Veprom() {};

        /*
        creates a new virtual EPROM chip with a specified capacity
        - size = specified capacity
        - filename = output path to str
        */
        eRetVal create(size_t size, string & filename);

        /*
        loads a vEPROM file
        - filename = path to storage file
        */
        eRetVal load(string filename);

        /*
        writes a raw string of bytes to a specific address on the virtual EPROM chip
        - addr = address within vEPROM
        - data = string data to write
        */
        eRetVal write_raw(size_t addr, uint8_t* data, size_t length);

        /* 
        reads the values stored at a specific address and length on the virtual EPROM chip and outputs it on stdout
        - addr = address within vEPROM
        - length = len to read
        */
        eRetVal read_raw(size_t addr, uint8_t* buf, size_t length);

        /*
        writes a file to the virtual EPROM chip.
        */
        eRetVal write(string filename, uint8_t* buf, size_t length);

        /*
        lists the files on the virtual EPROM chip.
        */
        eRetVal list(vector<string> & files);

        /*
        reads a file from the virtual EPROM chip.
        */
        eRetVal read(string filename, uint8_t** pBuf, size_t* pLen);

        /*
        sets the EPROM back to its original state
        */
        eRetVal erase();

    private:

        struct sFileHeader
        {
            char filename[SZ_FILENAME_BUF];
            size_t length;
        };

        /*
        get the filename corresponding to user working context
        */
        string get_context();

        /*
        get next free position in drive
        */
        size_t get_free_pos();

        /*
        get veprom size
        */
        bool get_size(size_t* pSz);
};