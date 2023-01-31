#include <string>

using namespace std;

#define SZ_FILENAME_BUF 32
#define N_VEPROMS_ALLOWED 16
#define METHOD_CREATE "create"
#define METHOD_LOAD "load"
#define METHOD_WRITE_RAW "write_raw"
#define METHOD_READ_RAW "read_raw"

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
        eRetVal write_raw(size_t addr, string data);

        /* 
        reads the values stored at a specific address and length on the virtual EPROM chip and outputs it on stdout
        - addr = address within vEPROM
        - length = len to read
        */
        eRetVal read_raw(size_t addr, uint8_t* buf, size_t length);

    private:

        /*
        get the filename corresponding to user working context
        */
        string get_context();

};