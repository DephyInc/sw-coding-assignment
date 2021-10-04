#include "datafile.hpp"
#include "proto.hpp"


/*********************************************
                     clean
*********************************************/
void clean(DataFile* dataFiles, int nFiles)
{
    int i;

    for (i = 0; i < nFiles; i++)
    {
        delete[] dataFiles[i].dataRecords;
    }
    delete[] dataFiles;
}
