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

#define USAGE \
    "Usages:\n" \
    " >> veprom " METHOD_CREATE     " [size in KB]          \n" \
    " >> veprom " METHOD_LOAD       " [veprom_name]         \n" \
    " >> veprom " METHOD_WRITE_RAW  " [address] [data]      \n" \
    " >> veprom " METHOD_READ_RAW   " [address] [length]    \n" \
    " >> veprom " METHOD_WRITE      " [filenmae]            \n" \
    " >> veprom " METHOD_LIST       "                       \n" \
    " >> veprom " METHOD_READ       " [filenmae]            \n" \
    " >> veprom " METHOD_ERASE      "                       \n" \

class Veprom
{
    public:

        enum eRetVal
        {
            OK = 0,
            MissingArgs,
            InvalidArgs,
            NotSupported,
            InvalidMethod,
            NotFound,
            __UsageErrors__ = 0x80,
            MaxNumDrivesExceeded,
            ContextNotLoaded,
            OutOfBounds,
            DriveFull,
            FilenameTooLong,
            FileNotFound,
            __ApplicationErrors__ = 0xC0,
            CannotGetSize,
            MemoryAllocError,
            FileInterfaceError,
            NullPtr,
            __RuntimeErrors__,
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

        /*
        Get string description of error
        */
        static string err_descr(eRetVal ret);

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
        bool get_free_pos(size_t* pSz);

        /*
        get veprom size
        */
        bool get_size(size_t* pSz);
};