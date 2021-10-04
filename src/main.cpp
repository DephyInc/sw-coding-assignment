#include "datafile.hpp"
#include "proto.hpp"

#include <string>
#include <iostream>
#include <fstream>


void write_stuff(DataFile*);


/*********************************************
                     main
*********************************************/
int main(int argc, char** argv)
{
    DataFile* dataFiles;

    // Read input files
    dataFiles = read_input_files(argc, argv);

    write_stuff(dataFiles);

    // Clean up
    clean(dataFiles, argc - 1);

    return 0;
}


void write_stuff(DataFile* dataFiles)
{
    // Write out read in data to make sure it was read in correctly.
    int i;
    int j;
    std::string line;
    std::fstream outputFile;

    for (i=0; i<2; i++)
    {
        outputFile.open("myfile_" + std::to_string(i) + ".txt", std::ios::out);
        for (j = 0; j < dataFiles[i].nRecords; j++)
        {
            line = dataFiles[i].dataRecords[j].dateTime + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].timeStamp)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].xAccel)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].yAccel)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].zAccel)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].xGyro)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].yGyro)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].zGyro)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].strain)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].analog0)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].analog1)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].current)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].encDisp)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].encCont)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].encComm)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].vb)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].vg)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].temp)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].status1)) + "," +
                std::to_string(int(dataFiles[i].dataRecords[j].status2)) + "\n";
            outputFile << line;
            line.clear();
        }
        outputFile.close();
    }
}
