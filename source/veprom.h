#include <string>

using namespace std;

#define SZ_FILENAME_BUF 32
#define N_VEPROMS_ALLOWED 16
#define METHOD_CREATE "create"
#define METHOD_LOAD "load"

#define FILENAME_EXT ".map"
#define FILENAME_CONTEXT "veprom_context" FILENAME_EXT

class Veprom
{
    private:

        string m_strPath;  // path to file for veprom instance
        size_t m_size;          // size of vEPROM drive

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
            __AppErrors__ = 0xC0,
            FilenameBufferInvalid,
            FilenamesExhausted,
            MemoryAllocError,
            FileCreationError,
            ContextNotFound,
            CannotOpenContext,
            CannotWriteContext,
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
};