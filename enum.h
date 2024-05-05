// I am aware that I can make the functions that use these return type ReturnCode
// but I decided to keep them at an int return value, conceptually 
enum ReturnCodes : int
{
    SUCCESS = 0,
    INVALID_CREATE,
    INVALID_SIZE,
    FOPEN_ERROR,
    INVALID_LOAD,
    CHIP_NOT_FOUND,
};
