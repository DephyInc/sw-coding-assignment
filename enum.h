// I am aware that I can make the functions that use these return type ReturnCode
// but I decided to keep them at an int return value, conceptually 
enum ReturnCodes : int
{
    SUCCESS = 0,
    CHIP_FILES_FULL,
    CHIP_NOT_FOUND,
    CHIP_FILE_NOT_FOUND,
    FOPEN_ERROR,
    INVALID_ADDRESS,
    INVALID_CREATE,
    INVALID_LOAD,
    INVALID_READ,
    INVALID_READ_RAW,
    INVALID_SIZE,
    INVALID_WRITE,
    INVALID_WRITE_RAW,
};
