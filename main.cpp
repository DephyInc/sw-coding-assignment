// main.cpp :
//

#include "EPROM.h"

int main(int argc, char* argv[])
{
    EPROM EP_ROM;

    return EP_ROM.ProcessCMD(argc > 1 ? argv[1] : "",
                             argc > 2 ? argv[2] : "",
                             argc > 3 ? argv[3] : "");
}
