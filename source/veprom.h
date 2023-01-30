#include <string>

using namespace std;

#define SZ_FILENAME_BUF 32

#define METHOD_CREATE "create"

class Veprom
{
    private:

        string m_strPath;  // path to file for veprom instance
        size_t m_size;          // size of vEPROM drive

    public:

        enum eRetVal
        {
            OK = 0,
            __UserErrors__ = 0x80,
            RequireMethod,
            RequireSize,
            InvalidSize,
            SizeTooLarge,
            __RuntimeErrors__ = 0xC0,
            FilenameBufferInvalid,
            FilenamesExhausted,
            MemoryAllocError,
            FileCreationError,
        };

        Veprom() {};
        ~Veprom() {};

        /*
        creates a new virtual EPROM chip with a specified capacity
        - size = specified capacity
        - path = output path to str
        Returns string name
        - on error "ERROR"
        */
        eRetVal create(size_t size, string & filename);
};